#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

// "int a" is firstprivate
int foo1(int *a) {
	int res;

	if (!(*a)) return 0;
	--(*a);

	#pragma omp task shared(res)
	res = foo1(a);

	#pragma omp taskwait

	printf("%d\n", *a);

	return res + 1;
}

int main(void) {
	int a = 10;
	int res;
	#pragma omp parallel
	{
		#pragma omp single
		{
			#pragma omp task
			res = foo1(&a);

			#pragma omp taskwait

			printf("%d %d\n", res, a);
		}
	}
}
