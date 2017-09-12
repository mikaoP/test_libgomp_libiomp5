#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main(void) {
	size_t sum = 0;
	#pragma omp parallel num_threads(8)
	{
		#pragma omp critical
		++sum;
		#pragma omp atomic
		++sum;
	}
	printf("%zu\n", sum);
	
}
