#!/bin/bash
#SBATCH -J mc-weak-N2
#SBATCH -A def-sponsor00
#SBATCH -N 2
#SBATCH --ntasks-per-node=32
#SBATCH -t 00:05:00
#SBATCH -o results/mc_weak_N2_%j.out

module purge
source env/load_modules.sh

SAMPLES_PER_RANK=10000000          # e.g. 1e7 per rank
RANKS_PER_NODE=32                  # must match --ntasks-per-node
NODES=${SLURM_NNODES}              # provided by Slurm
TOTAL_RANKS=$((NODES * RANKS_PER_NODE))
TOTAL_SAMPLES=$((TOTAL_RANKS * SAMPLES_PER_RANK))
SEED=42

srun --mpi=pmix ./src/montecarlo_mpi \
    --samples "${TOTAL_SAMPLES}" \
    --seed "${SEED}" \
    --output "results/mc_weak_N2_${SLURM_JOB_ID}.csv"
