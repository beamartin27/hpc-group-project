#!/bin/bash
#SBATCH -J mc-baseline
#SBATCH -A def-sponsor00       
#SBATCH -N 1                      # 1 node
#SBATCH --ntasks-per-node=2       # match CPUS from sinfo for node[1-6] Magic Castle, checked with sinfo -o "%N %c %G"
#SBATCH -t 00:05:00
#SBATCH -o results/mc_baseline_%j.out

source env/load_modules.sh

TOTAL_SAMPLES=100000000           # 1e8, adjust later
SEED=42

srun --mpi=pmix ./src/mpi_hello \
    --samples "${TOTAL_SAMPLES}" \
    --seed "${SEED}" \
    --output "results/mc_baseline_${SLURM_JOB_ID}.csv"
