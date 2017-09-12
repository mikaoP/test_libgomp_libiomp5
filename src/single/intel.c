#include <stdlib.h>
#include <stdio.h>
#include "omp.h"
#include "libiomp.h"

static size_t sum = 0;

static ident_t loc1 = { .flags = KMP_IDENT_KMPC };
static ident_t loc2 = { .flags = KMP_IDENT_KMPC | KMP_IDENT_BARRIER_IMPL };

static void outline_func(kmp_int32 *global_tid, kmp_int32 *bound_tid, ...) {
	if (__kmpc_single(&loc1, *global_tid)) {
		++sum;
		__kmpc_end_single(&loc1, *global_tid);
	}
	__kmpc_barrier(&loc2, *global_tid);

	if (__kmpc_single(&loc1, *global_tid)) {
		++sum;
		__kmpc_end_single(&loc1, *global_tid);
	}
	__kmpc_barrier(&loc2, *global_tid);

	if (__kmpc_single(&loc1, *global_tid)) {
		++sum;
		__kmpc_end_single(&loc1, *global_tid);
	}
	__kmpc_barrier(&loc2, *global_tid);

	if (__kmpc_single(&loc1, *global_tid)) {
		++sum;
		__kmpc_end_single(&loc1, *global_tid);
	}
	__kmpc_barrier(&loc2, *global_tid);
}



int main(void) {
	__kmpc_begin(&loc1, 0); // flags is currently ignored

	__kmpc_fork_call(&loc1, 0, outline_func);

	__kmpc_end(&loc1);

	printf("%zu\n", sum);
}
