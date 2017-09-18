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
	printf("%p %p %p %p\n", *a, *b, *c, *d);
}

static void task1(void *args)
{
	struct task_args *task1_args = args; 
	int p_a = *(task1_args->a);
	int p_b = *(task1_args->b);
	int p_c = *(task1_args->c);
	int p_d = *(task1_args->d);
	foo1(&p_a, &p_b, &p_c, &p_d);
}

static void function(void *args)
{
	struct thread_args *ta = args;
	if (GOMP_single_start()) {
		struct task_args task1_args = { ta->a, ta->b, ta->c, ta->d };
		GOMP_task(&task1, &task1_args, NULL, sizeof(struct task_args), ALIGN_32, 1, 0, NULL, 0);
	}
	GOMP_barrier();
	printf("%p %p %p %p\n", *(ta->a), *(ta->b), *(ta->c), *(ta->d));
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
}
