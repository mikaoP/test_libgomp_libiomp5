#include <stdlib.h>
#include <stdio.h>
#include "omp.h"
#include "libiomp.h"

static ident_t loc1 = { .flags = KMP_IDENT_KMPC };
static ident_t loc2 = { .flags = KMP_IDENT_KMPC | KMP_IDENT_BARRIER_EXPL };

static kmp_critical_name lock = { 0 };

static void outline_func(kmp_int32 *global_tid, kmp_int32 *bound_tid, ...) {
	printf("0\n");
	__kmpc_barrier(&loc2, *global_tid);
	printf("1\n");
	__kmpc_barrier(&loc2, *global_tid);
	printf("2\n");
}



int main(void) {
	__kmpc_begin(&loc1, 0); // flags is currently ignored

	__kmpc_fork_call(&loc1, 0, outline_func);

	__kmpc_end(&loc1);
}
