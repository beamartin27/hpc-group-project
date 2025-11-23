// TEMPORARY TEST PROGRAM
// Used only to test MPI + Slurm setup. Will be replaced by montecarlo_mpi.c.

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    long long total_samples = 1000000; // default
    const char *output_path = "results/mc_baseline.csv";

    // very dumb arg parsing: --samples N --output PATH
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--samples") == 0 && i + 1 < argc) {
            total_samples = atoll(argv[++i]);
        } else if (strcmp(argv[i], "--output") == 0 && i + 1 < argc) {
            output_path = argv[++i];
        }
    }

    long long local_samples = total_samples / size;
    long long local_hits = 0;

    double t0 = MPI_Wtime();

    // tiny Monte Carlo π
    for (long long i = 0; i < local_samples; ++i) {
        double x = (double)rand() / RAND_MAX;
        double y = (double)rand() / RAND_MAX;
        if (x * x + y * y <= 1.0) local_hits++;
    }

    long long global_hits = 0;
    MPI_Reduce(&local_hits, &global_hits, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    double t1 = MPI_Wtime();

    if (rank == 0) {
        double pi_est = 4.0 * (double)global_hits / (double)(local_samples * size);
        double elapsed = t1 - t0;

        FILE *f = fopen(output_path, "w");
        if (!f) {
            fprintf(stderr, "Could not open output file %s\n", output_path);
        } else {
            fprintf(f, "ranks,total_samples,pi_estimate,elapsed_sec\n");
            fprintf(f, "%d,%lld,%.8f,%.6f\n",
                    size, (long long)(local_samples * size), pi_est, elapsed);
            fclose(f);
        }
    }

    // Still print the hello lines if you like
    printf("Hello from rank %d of %d\n", rank, size);

    MPI_Finalize();
    return 0;
}

/*
#include <mpi.h>
#include <stdio.h>

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    printf("Hello from rank %d of %d\n", rank, size);
    MPI_Finalize();
    return 0;
}
*/