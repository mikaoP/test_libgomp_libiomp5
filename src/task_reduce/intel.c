#include <stdlib.h>
#include <stdio.h>
#include "omp.h"

#include "libiomp.h"

struct kmp_task_foo1 {
	kmp_task_t task;
	void *p_tg;
};

struct task_shared_args {
	int *a;
	void **tg;
};

static ident_t loc1 = { .flags = KMP_IDENT_KMPC };
static ident_t loc2 = { .flags = KMP_IDENT_KMPC | KMP_IDENT_BARRIER_IMPL };

static void omp_task_task1_private_map(void *task, void **tg) {
	struct kmp_task_foo1 *tmp = task;
	*tg = tmp->p_tg;
}

static kmp_int32 task1(kmp_int32 thread_num, void *task) {
	struct kmp_task_foo1 *tmp = task;
        struct task_shared_args *tmp1 = (struct task_shared_args *)(tmp->task.shareds);
	void *p_tg;
	omp_task_task1_private_map(task, &p_tg);

	int *red_a = __kmpc_task_reduction_get_th_data(thread_num, p_tg, tmp1->a); 

	(*red_a)++;

	return 0;
}

static void task1_red_func(int *reduce_init) { *reduce_init = 0; }
static void task1_red_comb(int *shared_data, int *priv_data) {
    *shared_data += *priv_data;
}

static void outline_func(kmp_int32 *global_tid, kmp_int32 *bound_tid, int *a) {

	if (__kmpc_single(&loc1, *global_tid)) {

		__kmpc_taskgroup(&loc1, *global_tid);

		kmp_task_red_input_t task1_red[1];

		task1_red[0].reduce_shar = a;
		task1_red[0].reduce_size = sizeof(*a);
		task1_red[0].reduce_init = task1_red_func;
		task1_red[0].reduce_fini = 0; // No fini?
		task1_red[0].reduce_comb = task1_red_comb;
		task1_red[0].flags.lazy_priv = 0;
		task1_red[0].flags.reserved31 = 0;

		void *tg = __kmpc_task_reduction_init(*global_tid, 1, task1_red);

		struct kmp_task_foo1 *ret1 = (struct kmp_task_foo1 *)__kmpc_omp_task_alloc(&loc1,
                                                         *global_tid,
                                                         TASK_TIED,
                                                         sizeof(struct kmp_task_foo1),
                                                         sizeof(struct task_shared_args),
                                                         (kmp_routine_entry_t)&task1);

		struct task_shared_args *tmp = (struct task_shared_args *)(ret1->task.shareds);
		*tmp = (struct task_shared_args) { a, &tg };

		ret1->p_tg = *(tmp->tg);

		__kmpc_omp_task(&loc1, *global_tid, (kmp_task_t *)ret1);

		__kmpc_end_taskgroup(&loc1, *global_tid);
		__kmpc_end_single(&loc1, *global_tid);
	}
	__kmpc_barrier(&loc2, *global_tid);
}

int main(void) {
	int a = 0;

	__kmpc_begin(&loc1, 0); // flags is currently ignored

	__kmpc_fork_call(&loc1, 1, (kmpc_micro)outline_func, &a);

	__kmpc_end(&loc1);

	printf("%d\n", a);
}
