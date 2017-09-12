#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include "libgomp_g.h"

static size_t sum = 0;

static void function(void *dummy)
{
	GOMP_critical_start();
	++sum;
	GOMP_critical_end();

	__sync_fetch_and_add(&sum, 1);
}

int main() {
	GOMP_parallel_start(function, NULL, 0);
	function(NULL);
	GOMP_parallel_end();

	printf("%zu\n", sum);
}
