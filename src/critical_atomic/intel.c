#include <stdlib.h>
#include <stdio.h>
#include "omp.h"
#include "libiomp.h"

static size_t sum = 0;

static ident_t loc1 = { .flags = KMP_IDENT_KMPC };

static kmp_critical_name lock = { 0 };

static void outline_func(kmp_int32 *global_tid, kmp_int32 *bound_tid, ...) {
	__kmpc_critical(&loc1, *global_tid, &lock);
	++sum;
	__kmpc_end_critical(&loc1, *global_tid, &lock);

	__kmpc_atomic_fixed4_add(&loc1, *global_tid, &sum, 1);
}



int main(void) {
	__kmpc_begin(&loc1, 0); // flags is currently ignored

	__kmpc_fork_call(&loc1, 0, outline_func);

	__kmpc_end(&loc1);

	printf("%zu\n", sum);
}
