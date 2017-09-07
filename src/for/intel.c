#include <stdlib.h>
#include <stdio.h>
#include "omp.h"
#include "libiomp.h"

#define N 20
static int array[N];

static ident_t loc1 = { .flags = KMP_IDENT_KMPC };
static ident_t loc2 = { .flags = KMP_IDENT_KMPC | KMP_IDENT_BARRIER_IMPL };

static void outline_func(kmp_int32 *global_tid, kmp_int32 *bound_tid, ...) {
	enum sched_type sched = kmp_sch_static;
	kmp_int32 liter = 0;
	kmp_int32 lower = 0;
	kmp_int32 upper = N - 1;
	kmp_int32 stride = 1; // clang sets to 1, but i do not know why...
	kmp_int32 incr = 1;
	kmp_int32 chunk = 0; // Not used in this sched

	__kmpc_for_static_init_4(&loc1, *global_tid, sched, &liter, &lower, &upper, &stride, incr, chunk);
//	printf("%d, %d, %d, %d\n", liter, lower, upper, stride);
	for (size_t i = lower; i <= upper; i += incr) {
		array[i] = omp_get_thread_num();
	}
	__kmpc_for_static_fini(&loc1, *global_tid);
	__kmpc_barrier(&loc2, *global_tid);
}



int main(void) {
	kmp_int32 gtid;

	__kmpc_begin(&loc1, 0); // flags is currently ignored

	__kmpc_fork_call(&loc1, 0, outline_func);

	__kmpc_end(&loc1);

	for (size_t i = 0; i < N; ++i) {
		printf(" %d ", array[i]);
	}
	printf("\n");
}
