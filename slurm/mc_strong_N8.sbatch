#!/bin/bash
#SBATCH -J mc-strong-N8
#SBATCH -A def-sponsor00
#SBATCH -N 8
#SBATCH --ntasks-per-node=32     # same ranks per node
#SBATCH -t 00:05:00
#SBATCH -o results/mc_strong_N8_%j.out

module purge
source env/load_modules.sh

TOTAL_SAMPLES=1000000000         # fixed global samples (e.g. 1e9)
SEED=42

srun --mpi=pmix ./src/montecarlo_mpi \
    --samples "${TOTAL_SAMPLES}" \
    --seed "${SEED}" \
    --output "results/mc_strong_N8_${SLURM_JOB_ID}.csv"
