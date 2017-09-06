#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main(void) {
	#pragma omp parallel
	printf("Thread #%d\n", omp_get_thread_num());
}
