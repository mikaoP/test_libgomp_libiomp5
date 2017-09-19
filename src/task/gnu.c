#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include "libgomp_g.h"

#define ALIGN_32 4

struct thread_args {
	int *a;
	int *b;
	int *c;
	int *d;
};

struct task_args {
	int *a;
	int *b;
	int *c;
	int *d;
};

static void foo1(int *a, int *b, int *c, int *d) {
	*a = *b = *c = *d = 0xFF;
	printf("foo1: Thread #%d %x %x %x %x\n", omp_get_thread_num(), *a, *b, *c, *d);
}

static void task1(void *args)
{
	struct task_args *task1_args = args; 
	int p1_a = *(task1_args->a);
	int p1_b = *(task1_args->b);
	int p1_c = *(task1_args->c);
	int p1_d = *(task1_args->d);
	foo1(&p1_a, &p1_b, &p1_c, &p1_d);
}

static void function(void *args)
{
	struct thread_args *ta = args;

	int p_a = *(ta->a);
	int p_b = *(ta->b);
	int p_c = *(ta->c);
	int p_d = *(ta->d);

	p_a = p_b = p_c = p_d = omp_get_thread_num();

	// Single does not do copy
	if (GOMP_single_start()) {
		p_a = p_b = p_c = p_d = 0xEE;

		struct task_args task1_args = { &p_a, &p_b, &p_c, &p_d };
		GOMP_task(&task1, &task1_args, NULL, sizeof(struct task_args), ALIGN_32, 1, 0, NULL, 0);

		usleep(100);

		printf("After task: Thread #%d %x %x %x %x\n", omp_get_thread_num(), p_a, p_b, p_c, p_d);
	}
	GOMP_barrier();
	printf("After single: Thread #%d %x %x %x %x\n", omp_get_thread_num(), p_a, p_b, p_c, p_d);
}

int main() {
	int a = 0xAA;
	int b = 0xBB;
	int c = 0xCC;
	int d = 0xDD;

	struct thread_args ta = { &a, &b, &c, &d };
	GOMP_parallel_start(function, &ta, 0);
	function(&ta);
	GOMP_parallel_end();

	printf("Thread #%d %x %x %x %x\n", omp_get_thread_num(), a, b, c, d);
}
