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
	printf("%zx %zx %zx %zx\n", *a, *b, *c, *d);
}

static void omp_task_foo1_private_map(void *task, int **a, int **b, int **c, int **d) {
	struct kmp_task_foo1 *tmp = task;
	*a = &tmp->p_a;
	*b = &tmp->p_b;
	*c = &tmp->p_c;
	*d = &tmp->p_d;
}

// I am not sure about args...
static kmp_int32 task1(kmp_int32 thread_num, void *task) {
	int *p_a;
	int *p_b;
	int *p_c;
	int *p_d;
	//printf("%d = %d\n", thread_num, omp_get_thread_num());
	omp_task_foo1_private_map(task, &p_a, &p_b, &p_c, &p_d);
	foo1(p_a, p_b, p_c, p_d);
	return 0;
}

static void outline_func(kmp_int32 *global_tid, kmp_int32 *bound_tid, int *a, int *b, int *c, int *d) {
	if (__kmpc_single(&loc1, *global_tid)) {
		struct kmp_task_foo1 *ret1 = (struct kmp_task_foo1 *)__kmpc_omp_task_alloc(&loc1,
                                                         *global_tid,
                                                         TASK_EXPLICIT,
                                                         sizeof(struct kmp_task_foo1),
                                                         sizeof(struct task_shared_args),
                                                         (kmp_routine_entry_t)&task1);

		struct task_shared_args *tmp = (struct task_shared_args *)(ret1->task.shareds);
		*tmp = (struct task_shared_args) { a, b, c, d };

		ret1->p_a = *(tmp->a);
		ret1->p_b = *(tmp->b);
		ret1->p_c = *(tmp->c);
		ret1->p_d = *(tmp->d);

		__kmpc_omp_task(&loc1, *global_tid, (kmp_task_t *)ret1);

		__kmpc_end_single(&loc1, *global_tid);
	}
	__kmpc_barrier(&loc2, *global_tid);
	printf("%zx %zx %zx %zx\n", *a, *b, *c, *d);
}

int main(void) {
	int a = 0xAA;
	int b = 0xBB;
	int c = 0xCC;
	int d = 0xDD;

	__kmpc_begin(&loc1, 0); // flags is currently ignored

	__kmpc_fork_call(&loc1, 4, (kmpc_micro)outline_func, &a, &b, &c, &d);

	__kmpc_end(&loc1);
}
