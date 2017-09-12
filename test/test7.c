#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define N 20

int main(void) {
	size_t sum = 0;
	#pragma omp parallel num_threads(8)
	{
		#pragma omp single
		++sum;
		#pragma omp single
		++sum;
		#pragma omp single
		++sum;
	}
	printf("%zu\n", sum);
}
