#include <stdlib.h>
#include <stdio.h>
#include "omp.h"

#include "libiomp.h"

#define TASK_FINAL (1 << 1)

static ident_t loc1 = { .flags = KMP_IDENT_KMPC };
static ident_t loc2 = { .flags = KMP_IDENT_KMPC | KMP_IDENT_BARRIER_IMPL };

// I am not sure about args...
static kmp_int32 task1(kmp_int32 thread_num, void *task) {
	printf("hola\n");
	return 0;
}

// I am not sure about args...
static kmp_int32 task2(kmp_int32 thread_num, void *task) {
	printf("aloh\n");
	return 0;
}

static void outline_func(kmp_int32 *global_tid, kmp_int32 *bound_tid) {
	kmp_task_t *ret;
	int is_final;
	// Single does not do copy
	if (__kmpc_single(&loc1, *global_tid)) {

		for (size_t i = 0; i < 10; ++i) {

			is_final = (i < 5) ? TASK_FINAL : !TASK_FINAL;
			ret = (kmp_task_t *)__kmpc_omp_task_alloc(&loc1,
	                                                         *global_tid,
	                                                         is_final | TASK_TIED,
	                                                         sizeof(kmp_task_t),
	                                                         0,
	                                                         (kmp_routine_entry_t)&task1);

			if (i >= 7) { // if clause
				__kmpc_omp_task_begin_if0(&loc1, *global_tid, ret);
				task1(*global_tid, ret);
				__kmpc_omp_task_complete_if0(&loc1, *global_tid, ret);
			} else {
				__kmpc_omp_task(&loc1, *global_tid, (kmp_task_t *)ret);
			}
		}

		ret = (kmp_task_t *)__kmpc_omp_task_alloc(&loc1,
                                                         *global_tid,
                                                         TASK_UNTIED,
                                                         sizeof(kmp_task_t),
                                                         0,
                                                         (kmp_routine_entry_t)&task2);
		ret->part_id = 0;

		__kmpc_omp_task(&loc1, *global_tid, (kmp_task_t *)ret);

		__kmpc_end_single(&loc1, *global_tid);
	}
	__kmpc_barrier(&loc2, *global_tid);
}

int main(void) {
	__kmpc_begin(&loc1, 0); // flags is currently ignored

	__kmpc_fork_call(&loc1, 0, (kmpc_micro)outline_func);

	__kmpc_end(&loc1);

}
