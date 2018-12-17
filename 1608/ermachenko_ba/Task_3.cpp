// labapar3.cpp: определяет точку входа для консольного приложения.
//Поразрядная сортировка для вещественных чисел (тип double) 

#include "stdafx.h"
#include <stdio.h>
#include "mpi.h"
#include <math.h>
#include <conio.h>
#include <time.h>
#include <stdlib.h>

// Функция radixPass принимает в качестве параметров
// номер байта Offset,
// число элементов N, 
// исходный массив source, 
// массив dest, куда будут записываться числа, отсортированные по байту Offset
// массив счетчиков count, соответствующий текущему проходу.
void swap(double *in, double *out) {
	delete[] in;
		in = out;
}
void radixPass(short Offset, long N, double *source, double *dest, long *count) {
	// временные переменные
	long *sp;
	long s, c, i, *cp;
	long *bp;


	// шаг 3
	s = 0; 	// временная переменная, хранящая сумму на данный момент
	cp = count;
	for (i = 256; i > 0; --i, ++cp) {
		c = *cp;
		*cp = s;
		s += c;
	}

	// шаг 4
	bp = 0;//bp = (char *)source + Offset;
	sp = 0;//sp = source;
	for (i = N; i > 0; --i, bp += sizeof(double), ++sp) {
		cp = count + *bp;
		dest[*cp] = *sp;
		++(*cp);
	}
}

void createCounters(double *data, long *counters, long N) {
	// i-й массив count расположен, начиная с адреса counters+256*i
	memset(counters, 0, 256 * sizeof(double) * sizeof(long));

	char *bp = (char*)data;
	char *dataEnd = (char*)(data + N);
	short i;

	while (bp != dataEnd) {
		// увеличиваем количество байт со значением *bp
		// i - текущий массив счетчиков
		for (i = 0; i<sizeof(double); i++)
			counters[256 * i + *bp++]++;
	}
}

// Функция для последнего прохода при поразрядной сортировке чисел с плавающей точкой
void floatRadixLastPass(short Offset, long N, double *source, double *dest, long *count) {
	double *sp;
	long s, c, i, *cp;
	char *bp;

	long numNeg = 0;
	for (i = 128; i<256; i++) numNeg += count[i];

	s = numNeg;
	cp = count;
	for (i = 0; i < 128; ++i, ++cp) {
		c = *cp;
		*cp = s;
		s += c;
	}

	// изменения, касающиеся обратного расположения отрицательных чисел.
	s = count[255] = 0;                 //
	cp = count + 254;                     //
	for (i = 254; i >= 128; --i, --cp) {//
		*cp += s;                       // остальное - то же, что и в
		s = *cp;                        // signedRadixLastPass
	}

	bp = (char *)source + Offset;
	sp = source;
	for (i = N; i > 0; --i, bp += sizeof(double), ++sp) {
		cp = count + *bp;
		if (*bp<128) dest[(*cp)++] = *sp;
		else dest[--(*cp)] = *sp;
	}
}


// поразрядная сортировка чисел с плавающей точкой
void floatRadixSort(double* &in, long N) {
	double *out =new double[N];
	short i;

	long *counters = new long[sizeof(double) * 256], *count;
	createCounters(in, counters, N);

	for (i = 0; i<sizeof(double) - 1; i++) {
		count = counters + 256 * i;
		if (count[0] == N) continue;
		radixPass(i, N, in, out, count);
		swap(in, out);
	}
	count = counters + 256 * i;
	floatRadixLastPass(i, N, in, out, count);

	delete in;
	in = out;
	delete counters;
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
int main(int argc, char* argv[])
{
	int ProcNum, ProcRank, RecvRank;
	int bufSize=0;
	int portion=0;
	int start_time = 0;
	int end_time = 0;
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
		scanf_s("%d", bufSize);
		if (bufSize <= 0) {
			printf("Error: Wrong number");
			return 0;
		}

		portion = bufSize / ProcNum;

		srand(time(0));
		arrayMain = new double[bufSize];

		Rand_arr(arrayMain, bufSize, section, frac);

	}

	MPI_Bcast(&portion, 1, MPI_INT, 0, MPI_COMM_WORLD);

	arrayLoc = new double[portion];

	MPI_Scatter(arrayMain, portion, MPI_DOUBLE, arrayLoc, portion, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	//vertion 1, liner
	if (ProcRank == 0) {
		start_time = MPI_Wtime();

		//for (int i = 0; i < NumberRandValues; i++) {
		//	globalSum += Function(arrayRandData[i]);
		//}
		end_time = MPI_Wtime();// end line

		printf("Liner program\n"); //show
		fflush(stdout);

		printf("Time = %.3f\n", (end_time - start_time));
		fflush(stdout);

		//double integral = (double)(max - min) / NumberRandValues * globalSum;
		//printf("Answer is:%.3f", integral);
		fflush(stdout);

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

