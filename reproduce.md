# Reproducibility Guide

This document details how to build, run, and analyze the Monte Carlo Pi simulation on the cluster.

## 1. Environment Setup

We use the standard software stack available on the cluster (Magic Castle / Compute Canada).

```bash
source env/load_modules.sh
```

**Modules loaded:**
- `StdEnv/2023`
- `gcc/12.3`
- `openmpi/4.1.5`

## 2. Building the Code

The source code is in `src/`. Use `make` to compile.

```bash
cd src
make
cd ..
```

This will produce the binary `src/montecarlo_mpi`.

## 3. Running Experiments

All Slurm scripts are in the `slurm/` directory. Output logs and CSVs will be saved to `results/`.

### 3.1 Baseline (1 Node)
Run this to verify correctness and get a baseline performance metric.

```bash
sbatch slurm/mc_baseline.sbatch
```

### 3.2 Strong Scaling
Fixed total problem size (10^9 samples), increasing number of nodes (2, 4, 6).

```bash
sbatch slurm/mc_strong_N2.sbatch
sbatch slurm/mc_strong_N4.sbatch
sbatch slurm/mc_strong_N6.sbatch
```

### 3.3 Weak Scaling
Fixed work per node, increasing total problem size with number of nodes.

```bash
sbatch slurm/mc_weak_N2.sbatch
sbatch slurm/mc_weak_N4.sbatch
sbatch slurm/mc_weak_N6.sbatch
```

## 4. Profiling

To analyze CPU usage and identify bottlenecks:

```bash
sbatch slurm/profile.sbatch
```

This runs `perf stat` and `perf record`. The output will be in `results/profile_<jobid>.out` and `results/perf_<jobid>.data`.

To analyze the perf data (interactive):
```bash
perf report -i results/perf_<jobid>.data
```

## 5. Generating Plots

We provide a Python script to parse the CSV results and generate scaling plots.

```bash
# Ensure you have python and pandas/matplotlib installed (or load a python module)
module load python/3.11
pip install pandas matplotlib --user

python src/plot_results.py
```

Plots will be saved in `results/plots/`.
