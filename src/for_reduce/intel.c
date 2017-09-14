#include <stdlib.h>
#include <stdio.h>
#include "omp.h"
#include "libiomp.h"

#define N 20
static int array[N];
struct reduction_sum {
	size_t sum;
};
static kmp_critical_name lock = { 0 };

static ident_t loc1 = { .flags = KMP_IDENT_KMPC };
static ident_t loc2 = { .flags = KMP_IDENT_KMPC | KMP_IDENT_BARRIER_IMPL };
static ident_t loc3 = { .flags = KMP_IDENT_KMPC | KMP_IDENT_ATOMIC_REDUCE };

void callback_reduction_sum(void *reduce_lhs, void *reduce_rhs) {
	((struct reduction_sum *)reduce_lhs)->sum += ((struct reduction_sum *)reduce_rhs)->sum;
}

static void outline_func(kmp_int32 *global_tid, kmp_int32 *bound_tid, size_t *sum, ...) {
	enum sched_type sched = kmp_sch_static;
	kmp_int32 liter = 0;
	kmp_int32 lower = 0;
	kmp_int32 upper = N - 1;
	kmp_int32 stride = 1; // clang sets to 1, but i do not know why...
	kmp_int32 incr = 1;
	kmp_int32 chunk = 0; // Not used in this sched

	struct reduction_sum local;
	local.sum = 0;

	__kmpc_for_static_init_4(&loc1, *global_tid, sched, &liter, &lower, &upper, &stride, incr, chunk);
//	printf("%d, %d, %d, %d\n", liter, lower, upper, stride);
	for (size_t i = lower; i <= upper; i += incr) {
		local.sum += omp_get_thread_num();
		usleep(100);
	}
	__kmpc_for_static_fini(&loc1, *global_tid);

	switch (__kmpc_reduce(&loc3, *global_tid, 1, sizeof(local), &local, callback_reduction_sum, &lock))
	{
	case 1:
		*sum += local.sum;
		__kmpc_end_reduce(&loc3, *global_tid, &lock);
		break;
	case 2:
		__kmpc_atomic_fixed4_add(&loc3, *global_tid, sum, local.sum);
		__kmpc_end_reduce(&loc3, *global_tid, &lock);
		break;
	default:
		break;
	}
	printf("Thread #%d: %zu\n", omp_get_thread_num(), *sum);
}



int main(void) {
	kmp_int32 gtid;
	size_t sum = 0;

	__kmpc_begin(&loc1, 0); // flags is currently ignored

	__kmpc_fork_call(&loc1, 1, (kmpc_micro)outline_func, &sum);

	__kmpc_end(&loc1);

}
