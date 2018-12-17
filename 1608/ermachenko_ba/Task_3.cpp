// labapar3.cpp: определяет точку входа для консольного приложения.
//Поразрядная сортировка для вещественных чисел (тип double) 

#include "stdafx.h"
#include <stdio.h>
#include "mpi.h"
#include <math.h>
#include <conio.h>
#include <time.h>
#include <stdlib.h>

void CountingSort(double *inp, double *out, int byteNum,
	int size)
{
	unsigned char *mas = (unsigned char *)inp;
	int counter[256];
	int tem;
	memset(counter, 0, sizeof(int) * 256);
	for (int i = 0; i<size; i++)
		counter[mas[8 * i + byteNum]]++;
	int j = 0;
	for (; j<256; j++)
	{
		if (counter[j] != 0)
			break;
	}
	tem = counter[j];
	counter[j] = 0;
	j++;
	for (; j<256; j++)
	{
		int b = counter[j];
		counter[j] = tem;
		tem += b;
	}
	for (int i = 0; i<size; i++)
	{
		out[counter[mas[8 * i + byteNum]]] = inp[i];
		counter[mas[8 * i + byteNum]]++;
	}
}

void LSDSortDouble(double *inp, int size)
{
	double *out = new double[size];
	CountingSort(inp, out, 0, size);
	CountingSort(out, inp, 1, size);
	CountingSort(inp, out, 2, size);
	CountingSort(out, inp, 3, size);
	CountingSort(inp, out, 4, size);
	CountingSort(out, inp, 5, size);
	CountingSort(inp, out, 6, size);
	CountingSort(out, inp, 7, size);
	delete[] out;
}

int Rand_arr(double* arrayMain, int bufSize, int section, int frac) {
	for (int i = 0; i < bufSize; i++)
	{
		double tmp = rand() % section;
		tmp += rand() % frac / (double)frac;
		arrayMain[i] = tmp;
	}
	return 1;
}

void show_array(double *arr, int size) {
	int n = 0;
	for (int i = 0; i < size; i++) {
		if (n++> 4) {
			n = 0;
			printf("%.3f\n ", arr[i]);
		}
		else
		{
			printf("%.3f  ", arr[i]);

		}
	}
	printf("\n");
	printf("\n");
}
int main(int argc, char* argv[])
{
	int ProcNum, ProcRank, RecvRank;
	int bufSize = 0;
	int portion = 0;
	double start_time = 0;
	double end_time = 0;
	const int section = 10000;
	const int frac = 1000;
	double *arrayMain = nullptr;
	double *arrayLoc = nullptr;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);

	if (ProcRank == 0) {
		printf("Enter size of array: ");
		fflush(stdout);
		scanf_s("%d", &bufSize);
		if (bufSize <= 0) {
			printf("Error: Wrong number");
			return 0;
		}

		portion = bufSize / ProcNum;

		srand(time(0));
		arrayMain = new double[bufSize];

		Rand_arr(arrayMain, bufSize, section, frac);
		//show_array(arrayMain, bufSize);
	}

	MPI_Bcast(&portion, 1, MPI_INT, 0, MPI_COMM_WORLD);

	arrayLoc = new double[portion];

	MPI_Scatter(arrayMain, portion, MPI_DOUBLE, arrayLoc, portion, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	//vertion 1, liner
	if (ProcRank == 0) {
		start_time = MPI_Wtime();

		LSDSortDouble(arrayMain, bufSize);


		printf("Liner program\n"); //show
		fflush(stdout);
		end_time = MPI_Wtime();// end line


		printf("Time = %.3f\n", (end_time - start_time));
		fflush(stdout);

		//double integral = (double)(max - min) / NumberRandValues * globalSum;
		//printf("Answer is:%.3f", integral);
		fflush(stdout);
		//show_array(arrayMain, bufSize);

		Rand_arr(arrayMain, bufSize, section, frac);
		MPI_Barrier(MPI_COMM_WORLD); //wait
	}

	else
	{
		MPI_Barrier(MPI_COMM_WORLD);
	}

	start_time = 0;
	end_time = 0;

	//version 2, parallel
	if (ProcRank == 0)
	{
		start_time = MPI_Wtime(); //start parallel
	}

	//for()
	if (ProcRank == 0)
	{
		end_time = MPI_Wtime();// end line

		printf("Parallel program\n"); //show
		fflush(stdout);

		printf("Time = %.3f\n", (end_time - start_time));
		fflush(stdout);

	}

	return 0;
}