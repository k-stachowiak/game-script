/* Copytight (C) 2015 Krzysztof Stachowiak */

#include "vm_test.h"

static void incr_r4_pre(struct MoonVm *vm)
{
	vm->registers[4] = deadcode;
}

static CELL_T incr_r4[] = {
	MOP_INCR,
	MR_R4,
	MOP_TERMINATE
};

static void incr_r4_post(struct TestContext *tc, struct MoonVm *vm)
{
	tc_record(tc,
		"Increment a register",
		vm->registers[4] == deadcode + 1);
}

/* ----- */

static void decr_sp_pre(struct MoonVm *vm) {}

static CELL_T decr_sp[] = {
	MOP_DECR,
	MR_SP,
	MOP_TERMINATE
};

static void decr_sp_post(struct TestContext *tc, struct MoonVm *vm)
{
	tc_record(tc,
		"Decrement a pointer",
		vm->stack_pointer == 2);
}

void vm_test_incr_decr(struct TestContext *tc)
{
	struct MoonVm vm;
	vm_init(&vm);
	vm_test_program(tc, &vm, incr_r4, LEN(incr_r4), incr_r4_pre, incr_r4_post);
	vm_test_program(tc, &vm, decr_sp, LEN(decr_sp), decr_sp_pre, decr_sp_post);
	vm_deinit(&vm);
}