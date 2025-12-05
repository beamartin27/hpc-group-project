#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <omp.h>
#include <stdint.h>

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

int main(int argc, char *argv[]) {
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    long long total_samples = 100000000; // default 100M
    const char *output_path = "results/mc_result.csv";
    int seed_base = 42;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--samples") == 0 && i + 1 < argc) {
            total_samples = atoll(argv[++i]);
        } else if (strcmp(argv[i], "--output") == 0 && i + 1 < argc) {
            output_path = argv[++i];
        } else if (strcmp(argv[i], "--seed") == 0 && i + 1 < argc) {
            seed_base = atoi(argv[++i]);
        }
    }

    long long local_samples = total_samples / size;
    if (rank < total_samples % size) {
        local_samples++;
    }

    long long local_hits = 0;

    // Set thread count from env or default to 1 if missing
    int num_threads = 1;
    #pragma omp parallel
    {
        #pragma omp single
        num_threads = omp_get_num_threads();
    }

    MPI_Barrier(MPI_COMM_WORLD);
    double t0 = MPI_Wtime();

    #pragma omp parallel reduction(+:local_hits)
    {
        int tid = omp_get_thread_num();
        // Seed initialization
        struct xorshift128_state state;
        uint64_t seed = seed_base + (rank * 1000) + tid;
        // Seed splitting (simple splitmix64-like init)
        uint64_t z = (seed + 0x9E3779B97F4A7C15ULL);
        z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
        z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
        state.s[0] = z ^ (z >> 31);
        
        z = (seed + 0x9E3779B97F4A7C15ULL + 0x9E3779B97F4A7C15ULL);
        z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
        z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
        state.s[1] = z ^ (z >> 31);

        long long my_samples = local_samples / num_threads;
        if (tid < local_samples % num_threads) {
            my_samples++;
        }

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

    long long global_hits = 0;
    MPI_Reduce(&local_hits, &global_hits, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    double t1 = MPI_Wtime();

    if (rank == 0) {
        double pi_est = 4.0 * (double)global_hits / (double)total_samples;
        double elapsed = t1 - t0;
        double compute_time = t_compute - t0;
        double comm_time = t1 - t_compute;

        FILE *f = fopen(output_path, "w");
        if (f) {
            fprintf(f, "ranks,threads,total_samples,pi_estimate,elapsed_sec,compute_sec,comm_sec\n");
            fprintf(f, "%d,%d,%lld,%.10f,%.6f,%.6f,%.6f\n",
                    size, num_threads, total_samples, pi_est, elapsed, compute_time, comm_time);
            fclose(f);
        }
        printf("Run completed: Pi = %.10f, Time = %.6f s\n", pi_est, elapsed);
    }

    MPI_Finalize();
    return 0;
}
