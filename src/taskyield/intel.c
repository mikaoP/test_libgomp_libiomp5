#include <stdlib.h>
#include <stdio.h>
#include "omp.h"

#include "libiomp.h"

omp_lock_t lck1;

struct kmp_task_task1 {
	kmp_task_t task;
};

struct task1_shared_args {
	size_t *sum;
};

static ident_t loc1 = { .flags = KMP_IDENT_KMPC };
static ident_t loc2 = { .flags = KMP_IDENT_KMPC | KMP_IDENT_BARRIER_IMPL };

// I am not sure about args...
static kmp_int32 task1(kmp_int32 thread_num, void *task) {
	struct kmp_task_task1 *tmp = task;
	struct task1_shared_args *tmp1 = tmp->task.shareds;
	while (!omp_test_lock(&lck1)) {
		__kmpc_omp_taskyield(&loc1, thread_num, 0);
	}

	(*(tmp1->sum))++;

	omp_unset_lock(&lck1);
	return 0;
}


static void outline_func(kmp_int32 *global_tid, kmp_int32 *bound_tid, size_t *sum) {
	// Single does not do copy
	if (__kmpc_single(&loc1, *global_tid)) {

		for (size_t i = 0; i < 100000; ++i) {

			struct kmp_task_task1 *ret1 = (struct kmp_task_task1 *)__kmpc_omp_task_alloc(&loc1,
	                                                         *global_tid,
	                                                         TASK_TIED,
	                                                         sizeof(struct kmp_task_task1),
	                                                         sizeof(struct task1_shared_args),
	                                                         (kmp_routine_entry_t)&task1);

			struct task1_shared_args *tmp = (struct task1_shared_args *)(ret1->task.shareds);
			*tmp = (struct task1_shared_args) { sum };

			__kmpc_omp_task(&loc1, *global_tid, (kmp_task_t *)ret1);
		}
		__kmpc_end_single(&loc1, *global_tid);
	}
	__kmpc_barrier(&loc2, *global_tid);
}

int main(void) {
	omp_init_lock(&lck1);
	size_t sum = 0;

	__kmpc_begin(&loc1, 0); // flags is currently ignored

	__kmpc_fork_call(&loc1, 1, (kmpc_micro)outline_func, &sum);

	__kmpc_end(&loc1);

	omp_destroy_lock(&lck1);
	printf("%zu\n", sum);
}
