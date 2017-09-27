#include <stdio.h>
#include <stdlib.h>
#include <omp.h>


int main(void) {
	int a = 0;
	#pragma omp parallel
	#pragma omp single
	{
		#pragma omp taskgroup
		{
			#pragma omp task
			{
				#pragma omp task
				{
					usleep(100);
					a = 1;
				}
			}
		}
		#pragma omp task
		a *= 10;
	}
	printf("%d\n", a);
}
