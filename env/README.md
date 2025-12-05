# env/ – Environment & Reproducibility

This project currently uses **cluster modules only** (no container yet).

## Modules (Magic Castle)

We load the following modules on the Magic Castle teaching cluster:

- `StdEnv/2023`
- `gcc/12.3`
- `openmpi/4.1.5`

They are loaded via:

```bash
source env/load_modules.sh
```

The script does:

 - module --force purge
 - module load StdEnv/2023
 - module load gcc/12.3
 - module load openmpi/4.1.5
 - module list

### Fresh setup: how to bootstrap the environment

On Magic Castle: 
```bash
cd ~/hpc-group-project          # or the path where you cloned the repo
source env/load_modules.sh

cd src
make                            # compiles the MPI code (currently mpi_hello.c)
cd ..

sbatch slurm/mc_baseline.sbatch
```
The baseline job will produce:

- results/mc_baseline_<JOBID>.csv – CSV with results
- results/mc_baseline_<JOBID>.out – Slurm output log