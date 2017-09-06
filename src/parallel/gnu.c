#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include "libgomp_g.h"

static void function(void *dummy)
{
	printf("%d\n", omp_get_thread_num());
}

int main() {
	GOMP_parallel_start(function, NULL, 0);
	function(NULL);
	GOMP_parallel_end();

	return 0;
}
