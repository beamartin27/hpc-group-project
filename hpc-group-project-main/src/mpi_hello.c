#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <omp.h>
#include <stdint.h>
#include <errno.h>

// Xorshift128+ RNG: Extremely fast and good quality for simulation
struct xorshift128_state {
    uint64_t s[2];
};

static inline uint64_t xorshift128plus(struct xorshift128_state *state) {
    uint64_t s1 = state->s[0];
    const uint64_t s0 = state->s[1];
    state->s[0] = s0;
    s1 ^= s1 << 23; // a
    state->s[1] = s1 ^ s0 ^ (s1 >> 18) ^ (s0 >> 5); // b, c
    return state->s[1] + s0;
}

// Convert uint64 to double in [0, 1)
static inline double to_double(uint64_t x) {
    return (x >> 11) * (1.0 / 9007199254740992.0);
}

// Print usage information
static void print_usage(const char *prog_name, int rank) {
    if (rank == 0) {
        fprintf(stderr, "Usage: %s --samples <int> --seed <int> --output <path>\n", prog_name);
        fprintf(stderr, "  --samples <int>  : Total number of Monte Carlo samples (required)\n");
        fprintf(stderr, "  --seed <int>     : Random seed for reproducibility (required)\n");
        fprintf(stderr, "  --output <path>  : Output CSV file path (required)\n");
    }
}

int main(int argc, char *argv[]) {
    int provided;
    int init_error = MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);
    if (init_error != MPI_SUCCESS) {
        fprintf(stderr, "Error: MPI_Init_thread failed\n");
        return 1;
    }

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Default values
    long long total_samples = -1;  // Must be provided
    const char *output_path = NULL;  // Must be provided
    int seed_base = -1;  // Must be provided

    // Parse command-line arguments
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--samples") == 0 && i + 1 < argc) {
            total_samples = atoll(argv[++i]);
            if (total_samples <= 0) {
                if (rank == 0) {
                    fprintf(stderr, "Error: --samples must be a positive integer\n");
                }
                MPI_Finalize();
                return 1;
            }
        } else if (strcmp(argv[i], "--output") == 0 && i + 1 < argc) {
            output_path = argv[++i];
        } else if (strcmp(argv[i], "--seed") == 0 && i + 1 < argc) {
            seed_base = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            print_usage(argv[0], rank);
            MPI_Finalize();
            return 0;
        }
    }

    // Validate required arguments
    if (total_samples <= 0 || output_path == NULL || seed_base < 0) {
        if (rank == 0) {
            fprintf(stderr, "Error: Missing required arguments\n");
            print_usage(argv[0], rank);
        }
        MPI_Finalize();
        return 1;
    }

    // Distribute samples across ranks (load balancing: extra samples go to lower ranks)
    long long local_samples = total_samples / size;
    if (rank < total_samples % size) {
        local_samples++;
    }

    if (rank == 0) {
        printf("Monte Carlo Pi Estimation\n");
        printf("  Total samples: %lld\n", total_samples);
        printf("  MPI ranks: %d\n", size);
        printf("  Seed: %d\n", seed_base);
        printf("  Output: %s\n", output_path);
    }

    long long local_hits = 0;

    // Detect OpenMP thread count
    int num_threads = 1;
    #pragma omp parallel
    {
        #pragma omp single
        num_threads = omp_get_num_threads();
    }

    if (rank == 0) {
        printf("  OpenMP threads per rank: %d\n", num_threads);
    }

    // Synchronize all ranks before timing
    MPI_Barrier(MPI_COMM_WORLD);
    double t0 = MPI_Wtime();

    // Monte Carlo computation with OpenMP parallelization
    #pragma omp parallel reduction(+:local_hits)
    {
        int tid = omp_get_thread_num();
        
        // Initialize RNG state with unique seed per rank+thread
        struct xorshift128_state state;
        uint64_t seed = (uint64_t)seed_base + ((uint64_t)rank * 1000ULL) + (uint64_t)tid;
        
        // SplitMix64-like initialization for better seed distribution
        uint64_t z = (seed + 0x9E3779B97F4A7C15ULL);
        z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
        z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
        state.s[0] = z ^ (z >> 31);
        
        z = (seed + 0x9E3779B97F4A7C15ULL + 0x9E3779B97F4A7C15ULL);
        z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
        z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
        state.s[1] = z ^ (z >> 31);

        // Distribute local_samples across threads
        long long my_samples = local_samples / num_threads;
        if (tid < local_samples % num_threads) {
            my_samples++;
        }

        // Monte Carlo loop: generate random points and count hits in unit circle
        long long my_hits = 0;
        for (long long i = 0; i < my_samples; ++i) {
            double x = to_double(xorshift128plus(&state));
            double y = to_double(xorshift128plus(&state));
            if (x * x + y * y <= 1.0) {
                my_hits++;
            }
        }
        local_hits += my_hits;
    }

    double t_compute = MPI_Wtime();

    // Reduce all local_hits to rank 0
    long long global_hits = 0;
    int reduce_error = MPI_Reduce(&local_hits, &global_hits, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
    if (reduce_error != MPI_SUCCESS) {
        if (rank == 0) {
            fprintf(stderr, "Error: MPI_Reduce failed\n");
        }
        MPI_Finalize();
        return 1;
    }

    double t1 = MPI_Wtime();

    // Rank 0 writes results to CSV
    if (rank == 0) {
        double pi_est = 4.0 * (double)global_hits / (double)total_samples;
        double elapsed = t1 - t0;
        double compute_time = t_compute - t0;
        double comm_time = t1 - t_compute;

        // Write CSV file
        FILE *f = fopen(output_path, "w");
        if (!f) {
            fprintf(stderr, "Error: Failed to open output file '%s': %s\n", output_path, strerror(errno));
            MPI_Finalize();
            return 1;
        }

        // Write CSV header and data
        fprintf(f, "ranks,threads,total_samples,pi_estimate,elapsed_sec,compute_sec,comm_sec\n");
        int write_error = fprintf(f, "%d,%d,%lld,%.10f,%.6f,%.6f,%.6f\n",
                size, num_threads, total_samples, pi_est, elapsed, compute_time, comm_time);
        
        if (write_error < 0) {
            fprintf(stderr, "Error: Failed to write to output file '%s': %s\n", output_path, strerror(errno));
            fclose(f);
            MPI_Finalize();
            return 1;
        }

        fclose(f);

        // Print summary to stdout
        printf("Results:\n");
        printf("  Pi estimate: %.10f\n", pi_est);
        printf("  Error: %.10f\n", pi_est - 3.14159265358979323846);
        printf("  Total time: %.6f s\n", elapsed);
        printf("  Compute time: %.6f s\n", compute_time);
        printf("  Communication time: %.6f s\n", comm_time);
        printf("  Output written to: %s\n", output_path);
    }

    MPI_Finalize();
    return 0;
}

