#include <stdlib.h>
#include <stdio.h>
#include "omp.h"

#include "libiomp.h"

static ident_t loc1 = { .flags = KMP_IDENT_KMPC };
static ident_t loc2 = { .flags = KMP_IDENT_KMPC | KMP_IDENT_BARRIER_IMPL };

struct kmp_task_task3 {
	kmp_task_t task;
};

struct task3_shared_args {
	char *b;
};


static void foo3(char *b) {
	*b *= 10;
}

// I am not sure about args...
static kmp_int32 task3(kmp_int32 thread_num, void *task) {
	struct kmp_task_task3 *tmp = task;
	struct task3_shared_args *tmp1 = tmp->task.shareds;
	foo3(tmp1->b);
	return 0;
}

struct kmp_task_task2 {
	kmp_task_t task;
};

struct task2_shared_args {
	char *a;
	char *b;
};


static void foo2(char *a, char *b) {
	usleep(200);
	*a *= 10;
	*b = 1;
}

// I am not sure about args...
static kmp_int32 task2(kmp_int32 thread_num, void *task) {
	struct kmp_task_task2 *tmp = task;
	struct task2_shared_args *tmp1 = tmp->task.shareds;
	foo2(tmp1->a, tmp1->b);
	return 0;
}

struct kmp_task_task1 {
	kmp_task_t task;
};

struct task1_shared_args {
	char *a;
};


static void foo1(char *a) {
	usleep(100);
	*a = 1;
}

// I am not sure about args...
static kmp_int32 task1(kmp_int32 thread_num, void *task) {
	struct kmp_task_task1 *tmp = task;
	struct task1_shared_args *tmp1 = tmp->task.shareds;
	foo1(tmp1->a);
	return 0;
}

static void outline_func(kmp_int32 *global_tid, kmp_int32 *bound_tid, char *a, char *b) {

	// Single does not do copy
	if (__kmpc_single(&loc1, *global_tid)) {

		// Task 1
		struct kmp_task_task1 *ret1 = (struct kmp_task_task1 *)__kmpc_omp_task_alloc(&loc1,
                                                         *global_tid,
                                                         TASK_EXPLICIT,
                                                         sizeof(struct kmp_task_task1),
                                                         sizeof(struct task1_shared_args),
                                                         (kmp_routine_entry_t)&task1);

		struct task1_shared_args *tmp1 = (struct task1_shared_args *)(ret1->task.shareds);
		*tmp1 = (struct task1_shared_args) { a };

		// kmp_depend_info_t.len seems not be used by the runtime.
		// clang sets to sizeof(var)
		kmp_depend_info_t info1 = { (kmp_intptr_t)a, sizeof(char), { 1, 1 } };
		__kmpc_omp_task_with_deps(&loc1, *global_tid, (kmp_task_t *)ret1, 1, &info1, 0, NULL);

		// Task2
		struct kmp_task_task2 *ret2 = (struct kmp_task_task2 *)__kmpc_omp_task_alloc(&loc1,
                                                         *global_tid,
                                                         TASK_EXPLICIT,
                                                         sizeof(struct kmp_task_task2),
                                                         sizeof(struct task2_shared_args),
                                                         (kmp_routine_entry_t)&task2);

		struct task2_shared_args *tmp2 = (struct task2_shared_args *)(ret2->task.shareds);
		*tmp2 = (struct task2_shared_args) { a, b };

		// kmp_depend_info_t.len seems not be used by the runtime.
		// clang sets to sizeof(var)
		kmp_depend_info_t info2[2];
		info2[0] = (kmp_depend_info_t) { (kmp_intptr_t)a, sizeof(char), { 1, 0 } };
		info2[1] = (kmp_depend_info_t) { (kmp_intptr_t)b, sizeof(char), { 1, 1 } };
		__kmpc_omp_task_with_deps(&loc1, *global_tid, (kmp_task_t *)ret2, 2, info2, 0, NULL);

		// Task 3
		struct kmp_task_task3 *ret3 = (struct kmp_task_task3 *)__kmpc_omp_task_alloc(&loc1,
                                                         *global_tid,
                                                         TASK_EXPLICIT,
                                                         sizeof(struct kmp_task_task3),
                                                         sizeof(struct task3_shared_args),
                                                         (kmp_routine_entry_t)&task3);

		struct task3_shared_args *tmp3 = (struct task3_shared_args *)(ret3->task.shareds);
		*tmp3 = (struct task3_shared_args) { b };

		// kmp_depend_info_t.len seems not be used by the runtime.
		// clang sets to sizeof(var)
		kmp_depend_info_t info3 = { (kmp_intptr_t)b, sizeof(char), { 1, 0 } };
		__kmpc_omp_task_with_deps(&loc1, *global_tid, (kmp_task_t *)ret3, 1, &info3, 0, NULL);

		__kmpc_end_single(&loc1, *global_tid);
	}
	__kmpc_barrier(&loc2, *global_tid);
}

int main(void) {
	char a = 0;
	char b = 0;

	__kmpc_begin(&loc1, 0); // flags is currently ignored

	__kmpc_fork_call(&loc1, 2, (kmpc_micro)outline_func, &a, &b);

	__kmpc_end(&loc1);

	printf("%d %d\n", a, b);
}
