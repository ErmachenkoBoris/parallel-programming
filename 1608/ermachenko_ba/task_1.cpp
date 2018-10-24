#include <stdio.h>
#include "mpi.h"
#include <math.h>
#include <conio.h>
#include <time.h>
#include <stdlib.h>

double Function(double x)
{
	return x + 1;
}

int main(int argc, char* argv[]) {
	int ProcNum, ProcRank, RecvRank;
	int N, min, max;
	double sum = 0;
	double rez = 0;
	double start_time = 0.0f, end_time = 0.0f;
	double global_sum = 0;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);

	start_time = MPI_Wtime();//begin
	srand(time(0));

	if (ProcRank == 0) {
		printf("Enter N");
		scanf_s("%d", &N);
		printf("Enter upper limit");
		scanf_s("%d", &max);
		printf("Enter lower limit");
		scanf_s("%d", &min);

	}

	MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&max, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&min, 1, MPI_INT, 0, MPI_COMM_WORLD);

	for (int i = 0; i < N / ProcNum; i++)
	{

		double tmp = rand() % (max - min) + min;
		tmp += rand() % 10000 / 10000.0;
		sum += Function(tmp);
	}

	MPI_Reduce(&sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	if (ProcRank == 0) {
		end_time = MPI_Wtime();// end
		printf("Time = %30.10f\n", (end_time - start_time) * 1000); //show

		double integral = (double)(max - min) / N * global_sum;
		printf("\n Answer is:%.3f", integral);
	}

	MPI_Finalize();

	return 0;
}