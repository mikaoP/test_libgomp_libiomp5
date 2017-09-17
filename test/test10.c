#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

void foo1(size_t *a, size_t *b, size_t *c, size_t *d) {
	*a = *b = *c = *d = 0xFF;
	printf("%zx %zx %zx %zx\n", *a, *b, *c, *d);
}

int main(void) {
	size_t a = 0xAA;
	size_t b = 0xBB;
	size_t c = 0xCC;
	size_t d = 0xDD;
	#pragma omp parallel firstprivate(a, b, c, d)
	{
		#pragma omp single
		{
			#pragma omp task
			foo1(&a, &b, &c, &d);
		}
		printf("%zx %zx %zx %zx\n", a, b, c, d);
	}
}
