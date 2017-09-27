#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define N 1000

int main(void) {
	size_t sum = 0;
	#pragma omp parallel
	#pragma omp single
	{
		#pragma omp taskloop grainsize(100)
		for (size_t i = 0; i < N; ++i) {
			#pragma omp critical
			sum += i;
		}
	}
	printf("%zu\n", sum);
}
