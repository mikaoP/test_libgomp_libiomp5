#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

void foo1(size_t *a, size_t *b, size_t *c, size_t *d) {
	*a = *b = *c = *d = 0xFF;
	printf("foo1: Thread #%d %zx %zx %zx %zx\n", omp_get_thread_num(), *a, *b, *c, *d);
}

int main(void) {
	size_t a = 0xAA;
	size_t b = 0xBB;
	size_t c = 0xCC;
	size_t d = 0xDD;
	#pragma omp parallel firstprivate(a, b, c, d)
	{
		a = b = c = d = omp_get_thread_num();

		#pragma omp single
		{
			a = b = c = d = 0xEE;

			#pragma omp task
			foo1(&a, &b, &c, &d);

			usleep(100);

			printf("After task: Thread #%d %zx %zx %zx %zx\n", omp_get_thread_num(), a, b, c, d);
		}
		printf("After single: Thread #%d %zx %zx %zx %zx\n", omp_get_thread_num(), a, b, c, d);
	}
	printf("Thread #%d %zx %zx %zx %zx\n", omp_get_thread_num(), a, b, c, d);
}
