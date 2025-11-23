#!/bin/bash
#SBATCH -J mc-baseline
#SBATCH -A def-sponsor00        
#SBATCH -N 1                      # 1 node
#SBATCH --ntasks-per-node=32      # TODO: match cores per node on Magic Castle, check with sinfo -o "%N %c %G" or scontrol show node | egrep "NodeName=|CPUTot="
#SBATCH -t 00:05:00
#SBATCH -o results/mc_baseline_%j.out

module purge
source env/load_modules.sh

TOTAL_SAMPLES=100000000           # 1e8, adjust later
SEED=42

srun --mpi=pmix ./src/montecarlo_mpi \
    --samples "${TOTAL_SAMPLES}" \
    --seed "${SEED}" \
    --output "results/mc_baseline_${SLURM_JOB_ID}.csv"
