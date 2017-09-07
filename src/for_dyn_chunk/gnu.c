#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include "libgomp_g.h"

#define N 20

static int array[N];

static void function(void *dummy)
{
	long start = 0;
	long end = N;
	long incr = 1;
	long chunk = 3;
	long _start, _end;

	if (GOMP_loop_dynamic_start(start, end, incr, chunk, &_start, &_end)) {
		do {
			for (size_t i = _start; i < _end; i += incr) {
				array[i] = omp_get_thread_num();
			}
		} while (GOMP_loop_dynamic_next(&_start, &_end));
	}
	GOMP_loop_end();
}

int main() {
	GOMP_parallel_start(function, NULL, 0);
	function(NULL);
	GOMP_parallel_end();

	for (size_t i = 0; i < N; ++i) {
		printf(" %d ", array[i]);
	}
	printf("\n");
	return 0;
}
