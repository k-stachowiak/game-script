/* Copytight (C) 2015 Krzysztof Stachowiak */

#include "vm_test.h"

static void add_sub_pre(struct MoonVm *vm)
{
	vm->registers[0] = 1;
	vm->registers[1] = 1;
}

static CELL_T add_sub[] = {
	MOP_ADD, MR_R1, MR_R0,
	MOP_PUT, MR_SP, MR_R1,
	MOP_SUB, MR_SP, MR_R0,
	MOP_TERMINATE
};

static void add_sub_post(struct TestContext *tc, struct MoonVm *vm)
{
	tc_record(tc,
		"Add and subtract integral numbers",
		vm->registers[0] == 1 &&
		vm->registers[1] == 2 &&
		vm->stack_pointer == 1);
}

/* ----- */

static void mul_div_pre(struct MoonVm *vm)
{
	vm->registers[0] = 2;
	vm->registers[1] = 2;
}

static CELL_T mul_div[] = {
	MOP_MUL, MR_R1, MR_R0,
	MOP_PUT, MR_SP, MR_R1,
	MOP_DIV, MR_SP, MR_R0,
	MOP_TERMINATE
};

static void mul_div_post(struct TestContext *tc, struct MoonVm *vm)
{
	tc_record(tc,
		"Multiply and divide integral numbers",
		vm->registers[0] == 2 &&
		vm->registers[1] == 4 &&
		vm->stack_pointer == 2);
}

/* ----- */

static void div_mod_pre(struct MoonVm *vm)
{
	vm->registers[0] = 5;
	vm->registers[1] = 5;
	vm->registers[2] = 2;
}

static CELL_T div_mod[] = {
	MOP_DIV, MR_R0, MR_R2,
	MOP_MOD, MR_R1, MR_R2,
	MOP_TERMINATE
};

static void div_mod_post(struct TestContext *tc, struct MoonVm *vm)
{
	tc_record(tc,
		"Find divisor and remainder",
		vm->registers[0] == 2 &&
		vm->registers[1] == 1);
}

/* ----- */

static void and_or_registers_pre(struct MoonVm *vm)
{
	vm->registers[0] = deadcode;
	vm->registers[1] = decoadde;
}

static CELL_T and_or_registers[] = {
	MOP_PUT, MR_R2, MR_R0,
	MOP_AND, MR_R2, MR_R0,
	MOP_PUT, MR_R3, MR_R1,
	MOP_OR, MR_R3, MR_R0,
	MOP_TERMINATE
};

static void and_or_registers_post(struct TestContext *tc, struct MoonVm *vm)
{
	tc_record(tc,
		"Performed logical operations on registers",
		vm->registers[2] == deadcode &&
		vm->registers[3] == deededde);
}

/* ----- */

static void xor_flag_pre(struct MoonVm *vm)
{
	vm->zero_flag = ca;
}

static CELL_T xor_flag[] = {
	MOP_XOR, MR_ZF, MR_ZF,
	MOP_TERMINATE
};

static void xor_flag_post(struct TestContext *tc, struct MoonVm *vm)
{
	tc_record(tc,
		"Performed logical operations on registers",
		vm->zero_flag == 0);
}

/* ----- */

void vm_test_arythmetic(struct TestContext *tc)
{
	struct MoonVm vm;
	vm_init(&vm);
	vm_test_program(tc, &vm, add_sub, LEN(add_sub), add_sub_pre, add_sub_post);
	vm_test_program(tc, &vm, mul_div, LEN(mul_div), mul_div_pre, mul_div_post);
	vm_test_program(tc, &vm, div_mod, LEN(div_mod), div_mod_pre, div_mod_post);
	vm_test_program(tc, &vm, and_or_registers, LEN(and_or_registers), and_or_registers_pre, and_or_registers_post);
	vm_test_program(tc, &vm, xor_flag, LEN(xor_flag), xor_flag_pre, xor_flag_post);
	vm_deinit(&vm);
}