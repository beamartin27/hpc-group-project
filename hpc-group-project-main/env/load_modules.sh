#!/usr/bin/env bash
# env/load_modules.sh
# Load the exact modules used for the Monte Carlo MPI project on Magic Castle.

# Start from a clean slate
module --force purge

# Base environment + compiler + MPI (from Magic Castle StdEnv/2023)
module load StdEnv/2023
module load gcc/12.3
module load openmpi/4.1.5

echo "=== Loaded modules for HPC Monte Carlo project ==="
module list
