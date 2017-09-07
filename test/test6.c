#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define N 20


int main(void) {
	size_t sum = 0;
	#pragma omp parallel num_threads(8)
	{
		#pragma omp for reduction(+:sum)
		for (size_t i = 0; i < N; i++) sum += omp_get_thread_num();
	}
	printf("%zu\n", sum);
}
