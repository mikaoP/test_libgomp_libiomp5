#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include "libgomp_g.h"

static void function(void *dummy)
{
	printf("0\n");
	GOMP_barrier();
	printf("1\n");
	GOMP_barrier();
	printf("2\n");
}

int main() {
	GOMP_parallel_start(function, NULL, 0);
	function(NULL);
	GOMP_parallel_end();
}
