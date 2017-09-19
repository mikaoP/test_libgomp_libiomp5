#include <stdlib.h>
#include <stdio.h>
#include "omp.h"

#include "libiomp.h"

static int foo1(int *a);

static ident_t loc1 = { .flags = KMP_IDENT_KMPC };
static ident_t loc2 = { .flags = KMP_IDENT_KMPC | KMP_IDENT_BARRIER_IMPL };

struct kmp_task_task2 {
	kmp_task_t task;
	int *p_a;
};

struct task2_shared_args {
	int **a;
	int *res;
};
static void omp_task_task2_private_map(void *task, int **a) {
	struct kmp_task_task2 *tmp = task;
	*a = tmp->p_a;
}

static kmp_int32 task2(kmp_int32 thread_num, void *task) {
	int *p_a;
	omp_task_task2_private_map(task, &p_a);
	struct kmp_task_task2 *tmp = task;
	struct task2_shared_args *tmp1 = tmp->task.shareds;
	*(tmp1->res) = foo1(p_a);
}

static int foo1(int *a) {
	int res;

	if (!(*a)) return 0;
	--(*a);

	int global_tid = __kmpc_global_thread_num(&loc1);
	struct kmp_task_task2 *ret1 = (struct kmp_task_task2 *)__kmpc_omp_task_alloc(&loc1,
                                                 global_tid,
                                                 TASK_EXPLICIT,
                                                 sizeof(struct kmp_task_task2),
                                                 sizeof(struct task2_shared_args),
                                                 (kmp_routine_entry_t)&task2);

	struct task2_shared_args *tmp = (struct task2_shared_args *)(ret1->task.shareds);
	*tmp = (struct task2_shared_args) { &a, &res };
	ret1->p_a = *(tmp->a);

	__kmpc_omp_task(&loc1, global_tid, (kmp_task_t *)ret1);
	__kmpc_omp_taskwait(&loc1, global_tid);

	printf("%d\n", *a);

	return res + 1;
}

struct kmp_task_task1 {
	kmp_task_t task;
};

struct task1_shared_args {
	int *a;
	int *res;
};

// I am not sure about args...
static kmp_int32 task1(kmp_int32 thread_num, void *task) {
	struct kmp_task_task1 *tmp = task;
	struct task1_shared_args *tmp1 = tmp->task.shareds;
	*(tmp1->res) = foo1(tmp1->a);
	return 0;
}

static void outline_func(kmp_int32 *global_tid, kmp_int32 *bound_tid, int *a, int *res) {
	if (__kmpc_single(&loc1, *global_tid)) {
		struct kmp_task_task1 *ret1 = (struct kmp_task_task1 *)__kmpc_omp_task_alloc(&loc1,
                                                         *global_tid,
                                                         TASK_EXPLICIT,
                                                         sizeof(struct kmp_task_task1),
                                                         sizeof(struct task1_shared_args),
                                                         (kmp_routine_entry_t)&task1);

		struct task1_shared_args *tmp = (struct task1_shared_args *)(ret1->task.shareds);
		*tmp = (struct task1_shared_args) { a, res };

		__kmpc_omp_task(&loc1, *global_tid, (kmp_task_t *)ret1);
		__kmpc_omp_taskwait(&loc1, *global_tid);

		printf("%d %d\n", *res, *a);

		__kmpc_end_single(&loc1, *global_tid);
	}
	__kmpc_barrier(&loc2, *global_tid);
}

int main(void) {
	int a = 10;
	int res;

	__kmpc_begin(&loc1, 0); // flags is currently ignored

	__kmpc_fork_call(&loc1, 4, (kmpc_micro)outline_func, &a, &res);

	__kmpc_end(&loc1);
}
