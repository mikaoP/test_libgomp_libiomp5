#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main(void) {
	#pragma omp parallel
	#pragma omp single
	{
		for (size_t i = 0; i < 10; ++i) {
			#pragma omp task final(i < 5) if(i < 7)
			printf("hola\n");
		}
		#pragma omp task untied
		printf("aloh\n");
	}
}
