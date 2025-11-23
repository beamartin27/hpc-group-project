# Slurm scripts for Monte Carlo project

## Scripts

- `mc_baseline.sbatch`: 1-node baseline run.
- `mc_strong_N2.sbatch`: strong-scaling run on 2 nodes (fixed total samples argument (--samples)).
- `mc_strong_N4.sbatch`: strong-scaling run on 4 nodes.
- `mc_strong_N8.sbatch`: strong-scaling run on 8 nodes.
- `mc_weak_N2.sbatch`: weak-scaling run on 2 nodes (fixed total samples argument (--samples)).
- `mc_weak_N4.sbatch`: weak-scaling run on 4 nodes.
- `mc_weak_N8.sbatch`: weak-scaling run on 8 nodes.

> Note: For weak scaling, we want constant work per rank: `TOTAL_SAMPLES = SAMPLES_PER_RANK × (nodes × ranks_per_node)`
> Let’s assume our program takes a total samples argument (--samples), and each rank will do total_samples / world_size internally.

Each script:
- Loads modules via `env/load_modules.sh`.
- Runs `./src/montecarlo_mpi` with `--samples`, `--seed`, `--output`.
- Writes output and logs into `results/`.

## Usage

From login node:

```bash
sbatch slurm/mc_baseline.sbatch

sbatch slurm/mc_strong_N2.sbatch
sbatch slurm/mc_strong_N4.sbatch
sbatch slurm/mc_strong_N8.sbatch

sbatch slurm/mc_weak_N2.sbatch
sbatch slurm/mc_weak_N4.sbatch
sbatch slurm/mc_weak_N8.sbatch
```

Timings can be obtained with

```bash
sacct -j <JOBID> --format=JobID,JobName,Elapsed,TotalCPU,AllocNodes
```

We can collect real experimet log timings in a structure way through
```bash
bash slurm/collect_sacct.sh 123456
```

> Note: Later the “experiments/results” person can take results/sacct_log.csv and plot stuff.
