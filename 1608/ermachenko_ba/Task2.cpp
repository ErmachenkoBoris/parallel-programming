// laba2parallel.cpp: определяет точку входа для консольного приложения.

#include "stdafx.h"
#include"mpi.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <mpi.h>

#define displacement(type, member) \
	  (((unsigned long) &((type *)0)->member) - (unsigned long)((type *)0))

double log2(double x);
int create_hypercube_topology(MPI_Comm old_comm, int n, MPI_Comm *new_comm);

double *tmp;

int main(int argc, char *argv[])
{
	int Procnam = 0;
	int Procrang = 0;
	int cw_size;
	int rank;

	//int ntri=1;
	//size_t buff_size = ntri * 9 * sizeof(double);
	//double *buffer = (double*)malloc(buff_size);

	MPI_Comm hc_comm;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &Procnam);
	MPI_Status Status;
	int hyper_dims = log2(Procnam);

	create_hypercube_topology(MPI_COMM_WORLD, hyper_dims, &hc_comm);
	MPI_Comm_rank(hc_comm, &Procrang);

	int i;
	int source, dest;
	for (i = 0; i < hyper_dims; i++) {
		MPI_Cart_shift(hc_comm, i, 1, &source, &dest);
		printf("Step %2d: I am process %3d and I will excange my buffer with %3d (source = %3d)\n", i, Procrang, dest, source);
		//printf("Example for this\n");
		//int bufEx = ;
		//if (Procrang == dest)
		//MPI_Send(&Procrang, 1, MPI_INT, dest, 0, hc_comm);

		//int RecvRank = 0;
		//MPI_Recv(&RecvRank, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, &hc_comm, &Status);
		//printf("\n Hello from process %3d", RecvRank);
		MPI_Barrier(hc_comm);
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
	int i=0;
	int *dims=(int*)malloc(n);
	int *period = (int*)malloc(n);
	for (i = 0; i < n; ++i) {
		dims[i] = 2;
		period[i] = 1;
	}
	int err = MPI_Cart_create(old_comm, n, dims, period, 0, new_comm);
	return err;
}
