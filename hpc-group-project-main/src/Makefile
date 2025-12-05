# src/Makefile
CC=mpicc
CFLAGS=-O3 -fopenmp -Wall -Wextra

all: mpi_hello

mpi_hello: mpi_hello.c
	$(CC) $(CFLAGS) -o mpi_hello mpi_hello.c

clean:
	rm -f mpi_hello
