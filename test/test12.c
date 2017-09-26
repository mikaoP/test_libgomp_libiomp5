#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

void foo1(char *a) {
	*a = 1;
}

void foo2(char *a, char *b) {
	*a *= 10;
	*b = 1;
}

void foo3(char *b) {
	*b *= 10;
}

int main(void) {
	char a = 0;
	char b = 0;
	#pragma omp parallel
	{
		#pragma omp single
		{
			#pragma omp task depend(out : a)
			foo1(&a);

			#pragma omp task depend(in : a) depend(out: b)
			foo2(&a, &b);

			#pragma omp task depend(out : b)
			foo3(&b);

		}
	}
	printf("%d %d\n", a, b);
}
