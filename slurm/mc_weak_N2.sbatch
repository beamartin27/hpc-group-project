#!/bin/bash
#SBATCH -J mc-weak-N2
#SBATCH -A def-sponsor00
#SBATCH -N 2
#SBATCH --ntasks-per-node=2
#SBATCH -t 00:05:00
#SBATCH -o results/mc_weak_N2_%j.out

source env/load_modules.sh

SAMPLES_PER_RANK=10000000          # e.g. 1e7 per rank
RANKS_PER_NODE=2                  # must match --ntasks-per-node
NODES=${SLURM_NNODES}              # provided by Slurm
TOTAL_RANKS=$((NODES * RANKS_PER_NODE))
TOTAL_SAMPLES=$((TOTAL_RANKS * SAMPLES_PER_RANK))
SEED=42

# Work around weird Slurm mem conflict
unset SLURM_MEM_PER_CPU SLURM_MEM_PER_GPU SLURM_MEM_PER_NODE

srun ./src/mpi_hello \
    --samples "${TOTAL_SAMPLES}" \
    --seed "${SEED}" \
    --output "results/mc_weak_N2_${SLURM_JOB_ID}.csv"
