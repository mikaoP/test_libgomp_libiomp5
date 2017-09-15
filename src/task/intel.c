#include <stdlib.h>
#include <stdio.h>
#include "omp.h"

#include "libiomp.h"

struct task_shared_args {
	int *a;
	int *b;
	int *c;
	int *d;
};

static ident_t loc1 = { .flags = KMP_IDENT_KMPC };
static ident_t loc2 = { .flags = KMP_IDENT_KMPC | KMP_IDENT_BARRIER_IMPL };

static void foo1(int *a, int *b, int *c, int *d) {
	printf("%x %x %x %x\n", *a, *b, *c, *d);
}

// I am not sure about args...
static kmp_int32 task1(kmp_int32 thread_num, void *task) {
	kmp_task_t *tmp = task;
	printf("%d = %d\n", thread_num, omp_get_thread_num());
	struct task_shared_args *tmp1 = tmp->shareds;
	foo1(tmp1->a, tmp1->b, tmp1->c, tmp1->d);
}

static void outline_func(kmp_int32 *global_tid, kmp_int32 *bound_tid, int *a, int *b, int *c, int *d) {
	if (__kmpc_single(&loc1, *global_tid)) {
		kmp_task_t *ret1 = __kmpc_omp_task_alloc(&loc1,
                                                         *global_tid,
                                                         TASK_EXPLICIT,
                                                         sizeof(kmp_task_t),
                                                         sizeof(struct task_shared_args),
                                                         (kmp_routine_entry_t)&task1);
		struct task_shared_args tmp = { a, b, c, d };
		ret1->shareds = &tmp;

		__kmpc_omp_task(&loc1, *global_tid, ret1);

		__kmpc_end_single(&loc1, *global_tid);
	}
	__kmpc_barrier(&loc2, *global_tid);
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
