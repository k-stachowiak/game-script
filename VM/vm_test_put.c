/* Copytight (C) 2015 Krzysztof Stachowiak */

#include "vm_test.h"

static void put_r0_r1_pre(struct MoonVm *vm)
{
	vm->registers[0] = 1;
	vm->registers[1] = deadcode;
}

static CELL_T put_r0_r1[] = {
	MOP_PUT,
	MR_R0,
	MR_R1,
	MOP_TERMINATE
};

static void put_r0_r1_post(struct TestContext *tc, struct MoonVm *vm)
{
	tc_record(tc,
		"Put register to register",
		vm->registers[0] == deadcode &&
		vm->registers[1] == deadcode);
}

/* ----- */

static void put_r2_zf_pre(struct MoonVm *vm)
{
	vm->registers[2] = 1;
	vm->zero_flag = ac;
}

static CELL_T put_r2_zf[] = {
	MOP_PUT,
	MR_R2,
	MR_ZF,
	MOP_TERMINATE
};

static void put_r2_zf_post(struct TestContext *tc, struct MoonVm *vm)
{
	tc_record(tc,
		"Put flag to register",
		vm->registers[2] == ac &&
		vm->zero_flag == ac);
}

/* ----- */

static void put_r3_1b_deref_pre(struct MoonVm *vm)
{
	vm->registers[3] = 1;
}

static CELL_T put_r3_1b_deref[] = {
	MOP_PUT,
	MR_R3,
	MR_DEREF,
	MR_1B,
	0x02, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, /* Pointing at the deref resource code */
	MOP_TERMINATE
};

static void put_r3_1b_deref_post(struct TestContext *tc, struct MoonVm *vm)
{
	tc_record(tc,
		"Put dereferenced to register",
		vm->registers[3] == MR_DEREF);
}

/* ----- */

static void put_r4_2b_literal_pre(struct MoonVm *vm)
{
	vm->registers[3] = 1;
}

static CELL_T put_r4_2b_literal[] = {
	MOP_PUT,
	MR_R4,
	MR_LITRL,
	MR_2B,
	0xEF,
	0xBE,
	MOP_TERMINATE
};

static void put_r4_2b_literal_post(struct TestContext *tc, struct MoonVm *vm)
{
	tc_record(tc,
		"Put literal to register",
		vm->registers[4] == 0xBEEF);
}

/* ----- */

static void put_of_r15_pre(struct MoonVm *vm)
{
	vm->registers[15] = ac;
	vm->overflow_flag = 1;
}

static CELL_T put_of_r15[] = {
	MOP_PUT,
	MR_OF,
	MR_R15,
	MOP_TERMINATE
};

static void put_of_r15_post(struct TestContext *tc, struct MoonVm *vm)
{
	tc_record(tc,
		"Put register to flag",
		vm->overflow_flag == ac);
}

/* ----- */

static void put_ip_8b_literal_pre(struct MoonVm *vm)
{
	vm->registers[0] = deadcode;
}

static CELL_T put_ip_8b_literal[] = {
	MOP_PUT,
	MR_IP,
	MR_LITRL,
	MR_8B,
	0xf, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, /* address of the terminate op */
	MOP_PUT,
	MR_R0,
	MR_IP,
	MOP_TERMINATE
};

static void put_ip_8b_literal_post(struct TestContext *tc, struct MoonVm *vm)
{
	tc_record(tc,
		"Put literal to pointer",
		vm->registers[0] == deadcode);
}

/* ----- */

static void put_addr_8b_literal_pre(struct MoonVm *vm)
{
	vm->registers[0] = deadcode;
}

static CELL_T put_addr_8b_literal[] = {
	MOP_PUT,
	MR_DEREF,
	MR_1B,
	0xe, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, /* address of the next op */
	MR_LITRL,
	MR_1B,
	MOP_TERMINATE,
	MOP_PUT,
	MR_R0,
	MR_LITRL,
	MR_1B,
	0,
	MOP_TERMINATE
};

static void put_addr_8b_literal_post(struct TestContext *tc, struct MoonVm *vm)
{
	tc_record(tc,
		"Put literal to dereferenced",
		vm->registers[0] == deadcode);
}

/* ----- */

void vm_test_put(struct TestContext *tc)
{
	struct MoonVm vm;
	vm_init(&vm);
	vm_test_program(tc, &vm, put_r0_r1, LEN(put_r0_r1), put_r0_r1_pre, put_r0_r1_post);
	vm_test_program(tc, &vm, put_r2_zf, LEN(put_r2_zf), put_r2_zf_pre, put_r2_zf_post);
	vm_test_program(tc, &vm, put_r3_1b_deref, LEN(put_r3_1b_deref), put_r3_1b_deref_pre, put_r3_1b_deref_post);
	vm_test_program(tc, &vm, put_r4_2b_literal, LEN(put_r4_2b_literal), put_r4_2b_literal_pre, put_r4_2b_literal_post);
	vm_test_program(tc, &vm, put_of_r15, LEN(put_of_r15), put_of_r15_pre, put_of_r15_post);
	vm_test_program(tc, &vm, put_ip_8b_literal, LEN(put_ip_8b_literal), put_ip_8b_literal_pre, put_ip_8b_literal_post);
	vm_test_program(tc, &vm, put_addr_8b_literal, LEN(put_addr_8b_literal), put_addr_8b_literal_pre, put_addr_8b_literal_post);
	vm_deinit(&vm);
}
