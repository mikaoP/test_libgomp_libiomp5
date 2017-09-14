#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main(void) {
	#pragma omp parallel num_threads(8)
	{
		printf("0\n");
		#pragma omp barrier
		printf("1\n");
		#pragma omp barrier
		printf("2\n");
	}
}
