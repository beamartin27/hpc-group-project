# HPC Group Project: Monte Carlo Pi Simulation

This project implements a scalable Monte Carlo simulation to estimate the value of Pi ($\pi$) using high-performance computing techniques.

## Features
-   **Hybrid Parallelism**: Uses MPI (Inter-node) and OpenMP (Intra-node) for maximum efficiency.
-   **Optimized RNG**: Replaces standard `rand()` with a fast, thread-safe `Xorshift128+` generator.
-   **Automated Profiling**: Includes Slurm scripts for performance analysis using `perf`.
-   **Scaling Analysis**: Scripts to measure Strong and Weak scaling across multiple nodes.

## Quick Start Guide

### 1. Connect to the Cluster
You must run this project on the Magic Castle cluster (or similar HPC environment).
```bash
ssh userXX@login1.hpcie.labs.faculty.ie.edu
cd hpc-group-project
```

### 2. Build the Code
Load the necessary modules and compile the source.
```bash
# Load GCC and OpenMPI
source env/load_modules.sh

# Compile the optimized binary
cd src
make clean
make
cd ..
```
*Output should show `mpicc ... -fopenmp ...`*

### 3. Run Experiments
Submit the Slurm jobs to the cluster queue.
```bash
# 1. Profiling (Performance counters)
sbatch slurm/profile.sbatch

# 2. Strong Scaling (Fixed total problem size, increasing nodes)
sbatch slurm/mc_strong_N2.sbatch
sbatch slurm/mc_strong_N4.sbatch
sbatch slurm/mc_strong_N6.sbatch

# 3. Weak Scaling (Fixed work per node, increasing nodes)
sbatch slurm/mc_weak_N2.sbatch
sbatch slurm/mc_weak_N4.sbatch
sbatch slurm/mc_weak_N6.sbatch
```

### 4. Monitor Jobs
Check the status of your jobs:
```bash
squeue -u $USER
```
Wait until the list is empty (all jobs completed).

### 5. Analyze Results
Once jobs are finished, download the results to your local machine to generate plots.

**On your LOCAL computer (PowerShell):**
```powershell
# Create a folder for results
mkdir results_final

# Download from cluster
scp -r userXX@login1.hpcie.labs.faculty.ie.edu:~/hpc-group-project/results/* ./results_final/

# Generate Scaling Plots
# (Ensure you have python and pandas installed: pip install pandas matplotlib)
python src/plot_results.py
```

The plots will be generated in `results_final/plots/`.

## Directory Structure
-   `src/`: C source code and plotting scripts.
-   `slurm/`: Job submission scripts (*.sbatch).
-   `env/`: Environment setup scripts.
-   `results/`: Output logs and CSV data.
