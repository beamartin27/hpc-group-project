# HPC Group Project: Monte Carlo Pi Simulation

**Team Members:** [Your Names]
**Date:** [Date]

## Abstract
Briefly summarize the project, the problem (estimating Pi), the approach (Monte Carlo with MPI/OpenMP), and the key results (scaling performance).

## 1. Introduction
### 1.1 Problem Statement
Explain the Monte Carlo method for estimating Pi.
### 1.2 Objectives
- Implement a parallel solution using MPI and OpenMP.
- Measure strong and weak scaling on the cluster.
- Profile and optimize the application.

## 2. Implementation
### 2.1 Algorithm
Describe the parallel algorithm:
- Domain decomposition (splitting samples across ranks).
- Random number generation (drand48_r with unique seeds).
- Reduction (MPI_Reduce).
### 2.2 Technologies
- C language.
- MPI for distributed memory parallelism.
- OpenMP for shared memory parallelism (hybrid approach).

## 3. Performance Analysis
### 3.1 Experimental Setup
- Hardware: [Node specs from Magic Castle]
- Software: GCC 12.3, OpenMPI 4.1.5.
- Parameters: [Total samples, nodes used]

### 3.2 Strong Scaling
[Insert Strong Scaling Plot]
Analysis:
- Does it scale linearly?
- Where does efficiency drop? (Amdahl's Law, communication overhead).

### 3.3 Weak Scaling
[Insert Weak Scaling Plot]
Analysis:
- Gustafson's Law.
- Is efficiency maintained as problem size grows?

### 3.4 Profiling & Optimization
- **Baseline Profile:** [Mention hotspots found via perf]
- **Optimization:** Replaced `rand()` with `drand48_r` and added OpenMP.
- **Impact:** [Quantify speedup or correctness improvement]

## 4. Conclusion
Summarize findings. Discuss limitations and future work (e.g., GPU implementation).

## References
- MPI Documentation
- OpenMP Documentation
