/* Copytight (C) 2015 Krzysztof Stachowiak */

#include <stdio.h>
#include "vm_test.h"

static void push_r0_pre(struct MoonVm *vm)
{
	vm->registers[0] = deadcode;
}

static CELL_T push_r0[] = {
	MOP_PUSH,
	MR_R0,
	MOP_TERMINATE
};

static void push_r0_post(struct TestContext *tc, struct MoonVm *vm)
{
	tc_record(tc,
		"Push register on the stack",
		vm->stack.data[vm->stack_pointer - 8] == 0xde &&
		vm->stack.data[vm->stack_pointer - 7] == 0xc0 && 
		vm->stack.data[vm->stack_pointer - 6] == 0xad && 
		vm->stack.data[vm->stack_pointer - 5] == 0xde && 
		vm->stack.data[vm->stack_pointer - 4] == 0x0 && 
		vm->stack.data[vm->stack_pointer - 3] == 0x0 && 
		vm->stack.data[vm->stack_pointer - 2] == 0x0 && 
		vm->stack.data[vm->stack_pointer - 1] == 0x0);
}

/* ----- */

static void push_pop_swap_pre(struct MoonVm *vm)
{
	vm->registers[3] = ac;
	vm->registers[14] = ca;
}

static CELL_T push_pop_swap[] = {
	MOP_PUSH,
	MR_R14,
	MOP_PUSH,
	MR_R3,
	MOP_POP,
	MR_R14,
	MOP_POP,
	MR_R3,
	MOP_TERMINATE
};

static void push_pop_swap_post(struct TestContext *tc, struct MoonVm *vm)
{
	tc_record(tc,
		"Push pop stack based swap",
		vm->registers[3] == ca &&
		vm->registers[14] == ac);
}

/* ----- */

void vm_test_push_pop(struct TestContext *tc)
{
	struct MoonVm vm;
	vm_init(&vm);
	vm_test_program(tc, &vm, push_r0, LEN(push_r0), push_r0_pre, push_r0_post);
	vm_test_program(tc, &vm, push_pop_swap, LEN(push_pop_swap), push_pop_swap_pre, push_pop_swap_post);
	vm_deinit(&vm);
}