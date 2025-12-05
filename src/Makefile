# src/Makefile
CC=mpicc
CFLAGS=-O3 -fopenmp

all: montecarlo_mpi

montecarlo_mpi: montecarlo_mpi.c
	$(CC) $(CFLAGS) -o montecarlo_mpi montecarlo_mpi.c

clean:
	rm -f montecarlo_mpi
