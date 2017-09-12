#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include "libgomp_g.h"

static size_t sum = 0;

static void function(void *dummy)
{
	if (GOMP_single_start()) {
		++sum;
	}
	GOMP_barrier();

	if (GOMP_single_start()) {
		++sum;
	}
	GOMP_barrier();

	if (GOMP_single_start()) {
		++sum;
	}
	GOMP_barrier();

	if (GOMP_single_start()) {
		++sum;
	}
	GOMP_barrier();
}

int main() {
	GOMP_parallel_start(function, NULL, 0);
	function(NULL);
	GOMP_parallel_end();

	printf("%zu\n", sum);
}
