#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

void foo0() {
	printf("HOLS");
}

void foo1(int *a) {
	printf("%d", *a);
}

void foo2(int *a, int *b) {
	printf("%d %d", *a, *b);
}

void foo3(int *a, int *b, int *c) {
	printf("%d %d %d", *a, *b, *c);
}

void foo4(int *a, int *b, int *c, int *d) {
	printf("%d %d %d %d", *a, *b, *c, *d);
}

void foo5(int *a, int *b, int *c, int *d, int *e) {
	printf("%d %d %d %d %d", *a, *b, *c, *d, *e);
}

int main(void) {
	int a, b, c, d, e;
	#pragma omp parallel
	{
		#pragma omp single
		{
			#pragma omp task
			foo0();
			a = 1;
			#pragma omp task
			foo1(&a);
			a = 2;
			b = 3;
			#pragma omp task
			foo2(&a, &b);
			a = 3;
			b = 4;
			c = 5;
			#pragma omp task
			foo3(&a, &b, &c);
			a = 4;
			b = 5;
			c = 6;
			d = 7;
			#pragma omp task
			foo4(&a, &b, &c, &d);
			a = 5;
			b = 6;
			c = 7;
			d = 8;
			e = 9;
			#pragma omp task
			foo5(&a, &b, &c, &d, &e);
		}
	}
}
