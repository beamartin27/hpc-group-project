# src/ – Source code

## Current files

- `mpi_hello.c`: **Temporary MPI test program** used only to:
  - check that modules (`gcc`, `openmpi`) work on the cluster,
  - verify that our Slurm scripts (`slurm/mc_*.sbatch`) run correctly across nodes.

- `Makefile`:
  - currently builds the test binary `mpi_hello` with `mpicc`.
  - will be updated later to also build the real Monte Carlo executable
    (e.g. `montecarlo_mpi`).

## Final target (to be implemented)

The real project should provide an executable, for example:

```bash
./src/montecarlo_mpi --samples <int> --seed <int> --output <csv-path>
```

⚠️ **All Slurm scripts in slurm/ expect that interface.**