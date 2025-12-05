#!/bin/bash
#SBATCH -J mc-profile
#SBATCH -A def-sponsor00
#SBATCH -N 1
#SBATCH --ntasks-per-node=2
#SBATCH -t 00:10:00
#SBATCH -o results/profile_%j.out

source env/load_modules.sh

TOTAL_SAMPLES=100000000
SEED=42
unset SLURM_MEM_PER_CPU SLURM_MEM_PER_GPU SLURM_MEM_PER_NODE
export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK

# 1. Basic stats
echo "Running perf stat..."
srun perf stat -e cycles,instructions,cache-references,cache-misses \
    ./src/montecarlo_mpi --samples ${TOTAL_SAMPLES} --seed ${SEED} --output /dev/null

# 2. Record profile (sampling)
echo "Running perf record..."
# Note: perf record with MPI can be tricky. We often just profile one rank.
# Here we try to profile all, but output might be mixed or per-rank depending on version.
# A safer way is to wrap the command in a script that only runs perf on rank 0.
# For simplicity, we'll just run it directly and see what we get, or use --output to separate.

srun -n 1 --exclusive perf record -g -o results/perf_%j.data \
    ./src/montecarlo_mpi --samples ${TOTAL_SAMPLES} --seed ${SEED} --output /dev/null

echo "Profiling done. Analyze with: perf report -i results/perf_<jobid>.data"
