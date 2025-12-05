#!/bin/bash
#SBATCH -J mc-strong-N2
#SBATCH -A def-sponsor00
#SBATCH -N 2
#SBATCH --ntasks-per-node=2
     # same ranks per node
#SBATCH -t 00:05:00
#SBATCH -o results/mc_strong_N2_%j.out

source env/load_modules.sh

TOTAL_SAMPLES=1000000000         # fixed global samples (e.g. 1e9)
SEED=42

# Work around weird Slurm mem conflict
unset SLURM_MEM_PER_CPU SLURM_MEM_PER_GPU SLURM_MEM_PER_NODE

export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK

srun ./src/mpi_hello \
    --samples "${TOTAL_SAMPLES}" \
    --seed "${SEED}" \
    --output "results/mc_strong_N2_${SLURM_JOB_ID}.csv"
