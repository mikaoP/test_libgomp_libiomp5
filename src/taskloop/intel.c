#include <stdlib.h>
#include <stdio.h>
#include "omp.h"
#include "libiomp.h"

#define SCHED_NONE 0
#define SCHED_GRAINSIZE 1
#define SCHED_NUMTASK 2

#define N 1000

static ident_t loc1 = { .flags = KMP_IDENT_KMPC };
static ident_t loc2 = { .flags = KMP_IDENT_KMPC | KMP_IDENT_BARRIER_IMPL };

static kmp_critical_name lock = { 0 };

struct kmp_task_task1 {
	kmp_task_t task;
	kmp_uint64 p_lb;
	kmp_uint64 p_ub;
	kmp_uint64 p_st;
};

struct task1_shared_args {
	size_t *sum;
};

// I am not sure about args...
static kmp_int32 task1(kmp_int32 thread_num, void *task) {
	struct kmp_task_task1 *tmp = task;
	struct task1_shared_args *tmp1 = tmp->task.shareds;

	kmp_int32 lower = tmp->p_lb;
	kmp_int32 upper = tmp->p_ub;
	kmp_int32 incr = tmp->p_st;

	for (size_t i = lower; i <= upper; i += incr) {
		__kmpc_critical(&loc1, thread_num, &lock);

		*(tmp1->sum) += i;

		__kmpc_end_critical(&loc1, thread_num, &lock);
	}

	return 0;
}

static void outline_func(kmp_int32 *global_tid, kmp_int32 *bound_tid, size_t *sum) {
	if (__kmpc_single(&loc1, *global_tid)) {

		__kmpc_taskgroup(&loc1, *global_tid);

		struct kmp_task_task1 *ret1 = (struct kmp_task_task1 *)__kmpc_omp_task_alloc(&loc1,
                                                         *global_tid,
                                                         TASK_EXPLICIT,
                                                         sizeof(struct kmp_task_task1),
                                                         sizeof(struct task1_shared_args),
                                                         (kmp_routine_entry_t)&task1);

		struct task1_shared_args *tmp1 = (struct task1_shared_args *)(ret1->task.shareds);
		*tmp1 = (struct task1_shared_args) { sum };

		ret1->p_lb = 0;
		ret1->p_ub = 999;
		ret1->p_st = 1;

		kmp_int32 if_val = 1;
                kmp_int32 nogroup = 0;
                kmp_int32 sched = SCHED_GRAINSIZE;
                kmp_uint64 grainsize = 100;
		__kmpc_taskloop(&loc1, *global_tid, (kmp_task_t *)ret1, if_val, &(ret1->p_lb), &(ret1->p_ub), ret1->p_st, nogroup, sched, grainsize, NULL);

		__kmpc_end_taskgroup(&loc1, *global_tid);

		__kmpc_end_single(&loc1, *global_tid);
	}
	__kmpc_barrier(&loc2, *global_tid);
}

int main(void) {
	size_t sum = 0;
	
	__kmpc_begin(&loc1, 0); // flags is currently ignored

	__kmpc_fork_call(&loc1, 1, (kmpc_micro)outline_func, &sum);

	__kmpc_end(&loc1);

	printf("%zu\n", sum);
}
