#include <stdlib.h>
#include <stdio.h>
#include "omp.h"

#include "libiomp.h"

struct kmp_task_foo1 {
	kmp_task_t task;
	int p_a;
	int p_b;
	int p_c;
	int p_d;
};

struct task_shared_args {
	int *a;
	int *b;
	int *c;
	int *d;
};

static ident_t loc1 = { .flags = KMP_IDENT_KMPC };
static ident_t loc2 = { .flags = KMP_IDENT_KMPC | KMP_IDENT_BARRIER_IMPL };

static void foo1(int *a, int *b, int *c, int *d) {
	*a = *b = *c = *d = 0xFF;
	printf("foo1: Thread #%d %x %x %x %x\n", omp_get_thread_num(), *a, *b, *c, *d);
}

static void omp_task_task1_private_map(void *task, int *a, int *b, int *c, int *d) {
	struct kmp_task_foo1 *tmp = task;
	*a = tmp->p_a;
	*b = tmp->p_b;
	*c = tmp->p_c;
	*d = tmp->p_d;
}

// I am not sure about args...
static kmp_int32 task1(kmp_int32 thread_num, void *task) {
	int p_a;
	int p_b;
	int p_c;
	int p_d;
	//printf("%d = %d\n", thread_num, omp_get_thread_num());
	omp_task_task1_private_map(task, &p_a, &p_b, &p_c, &p_d);
	foo1(&p_a, &p_b, &p_c, &p_d);
	return 0;
}

static void outline_func(kmp_int32 *global_tid, kmp_int32 *bound_tid, int *a, int *b, int *c, int *d) {

	int p_a = *a;
	int p_b = *b;
	int p_c = *c;
	int p_d = *d;

	p_a = p_b = p_c = p_d = omp_get_thread_num();

	// Single does not do copy
	if (__kmpc_single(&loc1, *global_tid)) {

		p_a = p_b = p_c = p_d = 0xEE;

		struct kmp_task_foo1 *ret1 = (struct kmp_task_foo1 *)__kmpc_omp_task_alloc(&loc1,
                                                         *global_tid,
                                                         TASK_EXPLICIT,
                                                         sizeof(struct kmp_task_foo1),
                                                         sizeof(struct task_shared_args),
                                                         (kmp_routine_entry_t)&task1);

		struct task_shared_args *tmp = (struct task_shared_args *)(ret1->task.shareds);
		*tmp = (struct task_shared_args) { &p_a, &p_b, &p_c, &p_d };

		ret1->p_a = *(tmp->a);
		ret1->p_b = *(tmp->b);
		ret1->p_c = *(tmp->c);
		ret1->p_d = *(tmp->d);

		__kmpc_omp_task(&loc1, *global_tid, (kmp_task_t *)ret1);

		usleep(100);

		printf("After task: Thread #%d %x %x %x %x\n", omp_get_thread_num(), p_a, p_b, p_c, p_d);

		__kmpc_end_single(&loc1, *global_tid);
	}
	__kmpc_barrier(&loc2, *global_tid);
	printf("After single: Thread #%d %x %x %x %x\n", omp_get_thread_num(), p_a, p_b, p_c, p_d);
}

int main(void) {
	int a = 0xAA;
	int b = 0xBB;
	int c = 0xCC;
	int d = 0xDD;

	__kmpc_begin(&loc1, 0); // flags is currently ignored

	__kmpc_fork_call(&loc1, 4, (kmpc_micro)outline_func, &a, &b, &c, &d);

	__kmpc_end(&loc1);

	printf("Thread #%d %zx %zx %zx %zx\n", omp_get_thread_num(), a, b, c, d);
}
