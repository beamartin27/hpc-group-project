# Slurm & Reproducibility Guide (Magic Castle)

This document explains how to clone the repo on the Magic Castle teaching cluster,
build the code, and reproduce all baseline / strong / weak scaling runs.

## 1. Clone the repo in the right place

**Important:** `/home` is small and shared. The professor explicitly asked us to put
projects and data under `/project` or `/scratch`.

On Magic Castle:
```bash
# login as usual, then:
cd /project

# create a personal folder if it doesn't exist
mkdir -p /project/$USER
cd /project/$USER

# clone the repo
git clone https://github.com/beamartin27/hpc-group-project.git
cd hpc-group-project

# check structure
ls
# expect: src/ env/ slurm/ results/ docs/ etc.
```

If you ever update the repo from your laptop, just run:
```bash
cd /project/$USER/hpc-group-project && git pull
```

## 2. Load modules & build the MPI code

We use the cluster modules, defined in `env/load_modules.sh`.
```bash
cd /project/$USER/hpc-group-project

# load modules (StdEnv/2023, gcc/12.3, openmpi/4.1.5)
source env/load_modules.sh

# build the MPI mini-app
cd src
make
cd ..
```

This should produce `src/mpi_hello`. The interface is:
```bash
./src/mpi_hello --samples <TOTAL_SAMPLES> --seed <INT> --output <CSV_PATH>
```

Rank 0 writes a CSV with:
```
ranks,total_samples,pi_estimate,elapsed_sec
```

The rest of the ranks just participate in the Monte Carlo.

## 3. Baseline run (1 node, 2 ranks)

We use 1 CPU node with 2 ranks:
```bash
cd /project/$USER/hpc-group-project

sbatch slurm/mc_baseline.sbatch
squeue -u $USER          # watch until mc-baseline disappears
```

Output:
```bash
ls results
cat  results/mc_baseline_<JOBID>.out
head results/mc_baseline_<JOBID>.csv
```

You should see something like:
```
ranks,total_samples,pi_estimate,elapsed_sec
2,100000000,3.1414...,2.2...
```

**Note:** the `.out` may contain noisy warnings about PSM3 / libfabric from Open MPI.
These are a Magic Castle configuration quirk; the job still completes successfully and
we ignore them in our analysis.

Baseline config:
- Nodes: 1
- Tasks per node: 2
- Global samples: TOTAL_SAMPLES = 100000000 (1e8)
- Work per rank: 5e7 samples

## 4. Strong scaling runs (fixed global problem size)

For strong scaling, we keep the global number of samples fixed and increase the
number of nodes.

Scripts:
- `slurm/mc_strong_N2.sbatch`
- `slurm/mc_strong_N4.sbatch`
- `slurm/mc_strong_N6.sbatch` (we have 6 CPU nodes: node[1-6])

Each script follows this pattern:
```bash
#!/bin/bash
#SBATCH -J mc-strong-N2         # or N4, N6
#SBATCH -A def-sponsor00
#SBATCH -N 2                    # 2, 4, or 6 nodes
#SBATCH --ntasks-per-node=2     # 2 ranks per node
#SBATCH -t 00:05:00
#SBATCH -o results/mc_strong_N2_%j.out

source env/load_modules.sh

TOTAL_SAMPLES=1000000000        # 1e9, same for all strong runs
SEED=42

unset SLURM_MEM_PER_CPU SLURM_MEM_PER_GPU SLURM_MEM_PER_NODE

srun ./src/mpi_hello \
    --samples "${TOTAL_SAMPLES}" \
    --seed "${SEED}" \
    --output "results/mc_strong_N2_${SLURM_JOB_ID}.csv"
```

Run them:
```bash
cd /project/$USER/hpc-group-project

sbatch slurm/mc_strong_N2.sbatch
sbatch slurm/mc_strong_N4.sbatch
sbatch slurm/mc_strong_N6.sbatch
```

For each job ID:
```bash
cat  results/mc_strong_N*_JOBID_.out
head results/mc_strong_N*_JOBID_.csv
```

We use these CSVs + sacct to build strong scaling plots.

## 5. Weak scaling runs (constant work per rank)

For weak scaling, we keep work per rank constant and increase the number of nodes.

Baseline per-rank work:
- Baseline: 1 node × 2 ranks, TOTAL_SAMPLES = 100000000
- So SAMPLES_PER_RANK = 100000000 / 2 = 50000000 samples

Weak scaling scripts use:
```bash
SAMPLES_PER_RANK=50000000
RANKS_PER_NODE=2
NODES=${SLURM_NNODES}
TOTAL_RANKS=$((NODES * RANKS_PER_NODE))
TOTAL_SAMPLES=$((TOTAL_RANKS * SAMPLES_PER_RANK))
SEED=42
```

Example `slurm/mc_weak_N2.sbatch`:
```bash
#!/bin/bash
#SBATCH -J mc-weak-N2
#SBATCH -A def-sponsor00
#SBATCH -N 2
#SBATCH --ntasks-per-node=2
#SBATCH -t 00:05:00
#SBATCH -o results/mc_weak_N2_%j.out

source env/load_modules.sh

SAMPLES_PER_RANK=50000000
RANKS_PER_NODE=2
NODES=${SLURM_NNODES}
TOTAL_RANKS=$((NODES * RANKS_PER_NODE))
TOTAL_SAMPLES=$((TOTAL_RANKS * SAMPLES_PER_RANK))
SEED=42

unset SLURM_MEM_PER_CPU SLURM_MEM_PER_GPU SLURM_MEM_PER_NODE

srun ./src/mpi_hello \
    --samples "${TOTAL_SAMPLES}" \
    --seed "${SEED}" \
    --output "results/mc_weak_N2_${SLURM_JOB_ID}.csv"
```

Similarly for:
- `mc_weak_N4.sbatch` → `#SBATCH -N 4`, filenames `mc_weak_N4_...`
- `mc_weak_N6.sbatch` → `#SBATCH -N 6`, filenames `mc_weak_N6_...`

Run them:
```bash
sbatch slurm/mc_weak_N2.sbatch
sbatch slurm/mc_weak_N4.sbatch
sbatch slurm/mc_weak_N6.sbatch
```

And inspect the outputs in `results/`.

## 6. Collecting timing with sacct

We provide `collect_sacct.sh` to capture Slurm accounting data for each job.

Usage:
```bash
cd /project/$USER/hpc-group-project

./collect_sacct.sh <JOBID>
```

This appends a line to `results/sacct_log.csv` with:
- JobID
- JobName
- State
- Elapsed
- TotalCPU
- AllocNodes
- AllocCPUS

Example workflow:
```bash
# after a baseline run:
sbatch slurm/mc_baseline.sbatch
# suppose you get JOBID 3513

./collect_sacct.sh 3513
cat results/sacct_log.csv
```

These logs, plus the CSVs from each run, are used later by the
Data/Experiments and Paper roles to build scaling plots and write the report.

## 7. Known quirks

Open MPI on Magic Castle prints warnings about PSM3 / libfabric (OFI) like:
```
PSM3 can't open nic unit: 0 (err=23)
Open MPI failed an OFI Libfabric library call (fi_endpoint)...
```

Despite the warnings, the jobs complete successfully and CSVs are written. We
treat this as a cluster configuration issue and ignore the messages in our analysis.