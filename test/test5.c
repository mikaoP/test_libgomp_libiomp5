#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define N 20

int array[N];

int main(void) {
	#pragma omp parallel num_threads(8)
	{
		#pragma omp for schedule(dynamic, 3)
		for (size_t i = 0; i < N; i++) array[i] = omp_get_thread_num();
	}
	for (size_t i = 0; i < N; i++) printf(" %d ", array[i]);
}
