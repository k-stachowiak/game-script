/* Copytight (C) 2015 Krzysztof Stachowiak */

#include "error.h"
#include "vm_test.h"
#include "test.h"

void vm_test_program(
		struct TestContext *tc,
		struct MoonVm *vm,
		CELL_T *program,
		int program_size,
		void(*preconditions)(struct MoonVm*),
		void(*postconditions)(struct TestContext *tc, struct MoonVm*))
{
	preconditions(vm);
	vm_load(vm, program, program_size);
	tc_record(tc, "Running test code", vm_run(vm));
	postconditions(tc, vm);
	vm_reset(vm);
}

void vm_test(void)
{
	struct TestContext tc;

	atexit(err_reset);

	tc_init(&tc);	
	vm_test_put(&tc);
	vm_test_push_pop(&tc);
	vm_test_incr_decr(&tc);
	vm_test_arythmetic(&tc);
	tc_report(&tc);
	tc_deinit(&tc);
}