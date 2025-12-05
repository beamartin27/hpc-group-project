# hpc-group-project

## How to run on the cluster (ssh)

1) **Clone your GitHub repo once**

```bash
git clone https://github.com/<org-or-user>/HPC-GROUP-PROJECT.git
cd HPC-GROUP-PROJECT
```

2) **Build the code** (on the cluster)

```bash
cd src
make            # this will compile mpi_hello.c
cd ..
```

3) **Submit jobs**

```bash
sbatch slurm/mc_baseline.sbatch
sbatch slurm/mc_strong_N2.sbatch
sbatch slurm/mc_strong_N4.sbatch
```

When you change stuff on your laptop and push:

```bash
# on cluster, inside repo
git pull
```

> Build again if necessary, then sbatch again.