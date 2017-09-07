#include <stdlib.h>
#include <stdio.h>
#include "omp.h"
#include "libiomp.h"

static void outline_func(kmp_int32 *global_tid, kmp_int32 *bound_tid, ...) {
	printf("%d\n", omp_get_thread_num());
}

int main(void) {
	ident_t loc1;
	kmp_int32 gtid;

	__kmpc_begin(&loc1, 0); // flags is currently ignored

	// Not necessary in this example
	gtid = __kmpc_global_thread_num(&loc1);

	__kmpc_fork_call(&loc1, 0, outline_func);

	__kmpc_end(&loc1);
}
