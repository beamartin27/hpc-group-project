# SYSTEM.md – Cluster & Environment

## Cluster

- Magic Castle teaching cluster (Slurm scheduler)
- - Typical project directory: `/project/$USER/hpc-group-project`

## Node types

We use CPU-only nodes on the Magic Castle teaching cluster for this project.

From `sinfo -o "%N %c %m %G"`:

- CPU nodes: `node[1-8]` — 2 CPUs, 4 GB memory, no GPUs (`gpu:0`)
- GPU nodes available but not used: `gpu-node[1-2]` — 4 CPUs, 28 GB memory, 1 GPU per node (`gpu:1`)


### Software environment (modules)

 Loaded via source env/load_modules.sh:

 - StdEnv/2023
 - gcc/12.3 – C compiler used by mpicc
 - openmpi/4.1.5 – MPI implementation

### Build & run commands (summary)

We build the code with `make` in `src/` and submit jobs using the Slurm scripts in `slurm/` (baseline + strong/weak scaling).

For exact commands:
- Quickstart: see the top-level `README.md`
- Environment details: see `env/README.md`
  