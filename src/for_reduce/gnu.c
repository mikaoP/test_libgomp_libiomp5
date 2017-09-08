#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include "libgomp_g.h"

#define N 20
#define PADDING 16
#define NTHREADS 8

static int array[NTHREADS * PADDING];
struct thread_args {
	size_t total_sum;
	int *reduction_array;
};

static void function(void *dummy)
{
	struct thread_args *ta = dummy;
	size_t local_sum = 0;
	
	long start = 0;
	long end = N;
	long incr = 1;
	long chunk = 0;
	long _start, _end;
	// if chunk_size = 0 -> chunk_size = N/#threads ceil
	// this only ocurrs in STATIC
	if (GOMP_loop_static_start(start, end, incr, chunk, &_start, &_end)) {
		do {
			for (size_t i = _start; i < _end; i += incr) {
				local_sum += omp_get_thread_num();
			}
		} while (GOMP_loop_static_next(&_start, &_end));
	}
	ta->reduction_array[omp_get_thread_num() * PADDING] = local_sum;

	GOMP_loop_end();

	// Master thread
	if (omp_get_thread_num() == 0) {
		for (size_t i = 0; i < omp_get_num_threads(); ++i) {
			ta->total_sum += ta->reduction_array[i * PADDING];
		}
	}
}

int main() {
	size_t sum = 0;

	struct thread_args ta;
	ta.total_sum = sum;
	ta.reduction_array = array;

	GOMP_parallel_start(function, &ta, NTHREADS);
	function(&ta);
	GOMP_parallel_end();

	sum = ta.total_sum;

	printf("%zu\n", sum);
	return 0;
}
