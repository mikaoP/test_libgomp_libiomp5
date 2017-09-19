#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include "libgomp_g.h"

#define ALIGN_32 4

static int foo1(int *a);

struct task2_args {
	int **a;
	int *res;
};

static void task2(void *args)
{
	struct task2_args *t2_args = args; 
	int *p_a = *(t2_args->a);
	*(t2_args->res) = foo1(p_a);
}

static int foo1(int *a) {
	int res;

	if (!(*a)) return 0;
	--(*a);

	struct task2_args t2_args = { &a, &res };
	GOMP_task(&task2, &t2_args, NULL, sizeof(struct task2_args), ALIGN_32, 1, 0, NULL, 0);

	GOMP_taskwait();

	printf("%d\n", *a);

	return res + 1;

}

struct thread_args {
	int *a;
	int *res;
};

struct task1_args {
	int *a;
	int *res;
};

static void task1(void *args)
{
	struct task1_args *t1_args = args; 
	*(t1_args->res) = foo1(t1_args->a);
}

static void function(void *args)
{
	struct thread_args *ta = args;
	if (GOMP_single_start()) {
		struct task1_args t1_args = { ta->a, ta->res };
		GOMP_task(&task1, &t1_args, NULL, sizeof(struct task1_args), ALIGN_32, 1, 0, NULL, 0);

		GOMP_taskwait();

		printf("%d %d\n", *(ta->res), *(ta->a));
	}
	GOMP_barrier();
}

int main() {
	int a = 10;
	int res;

	struct thread_args ta = { &a, &res };
	GOMP_parallel_start(function, &ta, 0);
	function(&ta);
	GOMP_parallel_end();
}
