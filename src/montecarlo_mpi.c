#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <omp.h>

// Use drand48_r for reentrant random number generation (Linux/POSIX)
// If compiling on non-POSIX, you might need a different RNG or a shim.

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
    // Handle remainder if total_samples not divisible by size
    if (rank < total_samples % size) {
        local_samples++;
    }

    long long local_hits = 0;

    MPI_Barrier(MPI_COMM_WORLD);
    double t0 = MPI_Wtime();

    #pragma omp parallel reduction(+:local_hits)
    {
        int thread_id = omp_get_thread_num();
        // Unique seed per rank and per thread
        long seed_val = seed_base + (rank * 1000) + thread_id;
        
        struct drand48_data buffer;
        srand48_r(seed_val, &buffer);

        long long my_samples = local_samples / omp_get_num_threads();
        // Simple handling of remainder for threads
        int num_threads = omp_get_num_threads();
        if (thread_id < local_samples % num_threads) {
            my_samples++;
        }

        for (long long i = 0; i < my_samples; ++i) {
            double x, y;
            drand48_r(&buffer, &x);
            drand48_r(&buffer, &y);
            if (x * x + y * y <= 1.0) {
                local_hits++;
            }
        }
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
        if (!f) {
            fprintf(stderr, "Could not open output file %s\n", output_path);
        } else {
            // Header: ranks, threads, total_samples, pi_estimate, elapsed_sec, compute_sec, comm_sec
            fprintf(f, "ranks,threads,total_samples,pi_estimate,elapsed_sec,compute_sec,comm_sec\n");
            int max_threads = 1;
            #pragma omp parallel
            {
                #pragma omp single
                max_threads = omp_get_num_threads();
            }
            fprintf(f, "%d,%d,%lld,%.10f,%.6f,%.6f,%.6f\n",
                    size, max_threads, total_samples, pi_est, elapsed, compute_time, comm_time);
            fclose(f);
        }
        printf("Run completed: Pi = %.10f, Time = %.6f s\n", pi_est, elapsed);
    }

    MPI_Finalize();
    return 0;
}
