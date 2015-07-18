/* Copytight (C) 2015 Krzysztof Stachowiak */

#include "vm_test.h"

static void test_r0_zero_pre(struct MoonVm *vm) {
	vm->overflow_flag = 1;
	vm->carry_flag = 1;
	vm->zero_flag = 0;
	vm->registers[0] = 0;
}

static CELL_T test_r0_zero[] = {
	MOP_TEST, MR_R0, MR_R0,
	MOP_TERMINATE
};

static void test_r0_zero_post(struct TestContext *tc, struct MoonVm *vm) {
	tc_record(tc,
		"Proper test result on two 0 operands",
		vm->overflow_flag == 0 &&
		vm->carry_flag == 0 &&
		vm->zero_flag == 1);
}

/* ----- */

static void test_r14_nonzero_pre(struct MoonVm *vm) {
	vm->overflow_flag = 1;
	vm->carry_flag = 1;
	vm->zero_flag = 0;
	vm->registers[14] = 1;
}

static CELL_T test_r14_nonzero[] = {
	MOP_TEST, MR_R14, MR_R14,
	MOP_TERMINATE
};

static void test_r14_nonzero_post(struct TestContext *tc, struct MoonVm *vm) {
	tc_record(tc,
		"Proper test result on two non-0 operands",
		vm->overflow_flag == 0 &&
		vm->carry_flag == 0 &&
		vm->zero_flag == 0);
}

/* ----- */

static void test_r4_neg_pre(struct MoonVm *vm) {
	vm->overflow_flag = 1;
	vm->carry_flag = 1;
	vm->sign_flag = 0;
	vm->registers[4] = -1;
}

static CELL_T test_r4_neg[] = {
	MOP_TEST, MR_R4, MR_R4,
	MOP_TERMINATE
};

static void test_r4_neg_post(struct TestContext *tc, struct MoonVm *vm) {
	tc_record(tc,
		"Proper test result on two negative operands",
		vm->overflow_flag == 0 &&
		vm->carry_flag == 0 &&
		vm->sign_flag == 1);
}

/* ----- */

static void test_r7_nonneg_pre(struct MoonVm *vm) {
	vm->overflow_flag = 1;
	vm->carry_flag = 1;
	vm->sign_flag = 1;
	vm->registers[7] = 1;
}

static CELL_T test_r7_nonneg[] = {
	MOP_TEST, MR_R7, MR_R7,
	MOP_TERMINATE
};

static void test_r7_nonneg_post(struct TestContext *tc, struct MoonVm *vm) {
	tc_record(tc,
		"Proper test result on two non-negative operands",
		vm->overflow_flag == 0 &&
		vm->carry_flag == 0 &&
		vm->sign_flag == 0);
}

/* ----- */

void vm_test_flow(struct TestContext *tc)
{
	struct MoonVm vm;
	vm_init(&vm);
	vm_test_program(tc, &vm, test_r0_zero, LEN(test_r0_zero), test_r0_zero_pre, test_r0_zero_post);
	vm_test_program(tc, &vm, test_r14_nonzero, LEN(test_r14_nonzero), test_r14_nonzero_pre, test_r14_nonzero_post);
	vm_test_program(tc, &vm, test_r4_neg, LEN(test_r4_neg), test_r4_neg_pre, test_r4_neg_post);
	vm_test_program(tc, &vm, test_r7_nonneg, LEN(test_r7_nonneg), test_r7_nonneg_pre, test_r7_nonneg_post);
	vm_deinit(&vm);
}