// laba2parallel.cpp: определяет точку входа для консольного приложения.

#include "stdafx.h"
#include"mpi.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <mpi.h>

double log2(double x);
int create_hypercube_topology(MPI_Comm old_comm, int n, MPI_Comm *new_comm);

double *tmp;

int main(int argc, char *argv[])
{
	int Procnam = 0;
	int rang = 0;
	int cw_size;
	int rank;
	int mess = 0;

	MPI_Comm hc_comm;
	MPI_Status Status;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &Procnam);

	int hyper_dims = log2(Procnam);

	create_hypercube_topology(MPI_COMM_WORLD, hyper_dims, &hc_comm);

	MPI_Comm_rank(hc_comm, &rank);

	int i;

	int source = 0, dest = 0;
	for (i = 0; i < hyper_dims; i++) {
		MPI_Cart_shift(hc_comm, i, 1, &source, &dest);
		printf("Step %2d: I am process %3d and I will excange my buffer with %3d (source = %3d)\n", i, rank, dest, source);
		printf("___And I have present from %3d\n", mess);
		MPI_Barrier(hc_comm);
		int sendr = rank;
		MPI_Sendrecv(&sendr, 1, MPI_INT, source, 1, &mess, 1, MPI_INT, rank, 1, hc_comm, &Status);
	}
	MPI_Finalize();
	return 0;
}

double log2(double x)
{
	return log(x) / log(2);
}

int create_hypercube_topology(MPI_Comm old_comm, const int n, MPI_Comm *new_comm)
{
	int i = 0;
	int *dims = (int*)malloc(n);
	int *period = (int*)malloc(n);
	for (i = 0; i < n; ++i) {
		dims[i] = 2;
		period[i] = 1;
	}
	int err = MPI_Cart_create(old_comm, n, dims, period, 0, new_comm);
	re
