# Project Pitch: Scalable Monte Carlo Pi

## Slide 1: Title & Team
- Project Name: MC-PI-SCALE
- Team Members
- Goal: Calculate Pi using HPC

## Slide 2: The Problem
- Monte Carlo methods are computationally expensive.
- Need high precision -> Billions of samples.
- Solution: Parallelize across multiple nodes.

## Slide 3: Methodology
- **Hybrid Parallelism:** MPI (Inter-node) + OpenMP (Intra-node).
- **Optimization:** Thread-safe RNG (`drand48_r`), efficient reduction.
- **Infrastructure:** Slurm, GCC, OpenMPI.

## Slide 4: Results
- **Strong Scaling:** [X]x speedup on [N] nodes.
- **Efficiency:** Maintained >[Y]% efficiency.
- **Bottlenecks:** Communication overhead at large N.

## Slide 5: Conclusion
- Successfully demonstrated scalable Monte Carlo simulation.
- Ready for EuroHPC scale deployment.
- Future work: GPU acceleration (CUDA).
