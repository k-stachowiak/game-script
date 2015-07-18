/* Copytight (C) 2015 Krzysztof Stachowiak */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "error.h"
#include "vm.h"

#define MIN(x, y) ((x > y) ? y : x)

static void reg_ptr_add(REG_T *x, REG_T *y) { *x += *y; }
static void reg_ptr_sub(REG_T *x, REG_T *y) { *x -= *y; }
static void reg_ptr_mul(REG_T *x, REG_T *y) { *x *= *y; }
static void reg_ptr_div(REG_T *x, REG_T *y) { *x /= *y; }
static void reg_ptr_mod(REG_T *x, REG_T *y) { *x %= *y; }

static void reg_ptr_and(REG_T *x, REG_T *y) { *x &= *y; }
static void reg_ptr_or(REG_T *x, REG_T *y) { *x |= *y; }
static void reg_ptr_xor(REG_T *x, REG_T *y) { *x ^= *y; }

static void reg_ptr_not(REG_T *x) { *x = ~(*x);  }

static bool vm_op_put(struct MoonVm *vm)
{
	/* put ::= "put" named_resource resource */

	int dst_size, src_size;
	char *dst, *src;

	if (!(vm_next_named_resource(vm, vm_next(vm), &dst, &dst_size) &&
          vm_next_resource(vm, vm_next(vm), &src, &src_size))) {
		err_push("VM", "Failed reading resource for put operation");
		return false;
	}

	memset(dst, 0, dst_size);
	memcpy(dst, src, MIN(src_size, dst_size));

	return true;
}

static bool vm_op_push(struct MoonVm *vm)
{
	/* push ::= "push" resource */

	int src_size;
	char *src, *dst;

	if (!vm_next_resource(vm, vm_next(vm), &src, &src_size)) {
		err_push("VM", "Failed reading resource for push operation");
		return false;
	}

	vm_expand_stack(vm, src_size);
	dst = vm->stack.data + vm->stack_pointer;
	memcpy(dst, src, src_size);
	vm->stack_pointer += src_size;

	return true;
}

static bool vm_op_pop(struct MoonVm *vm)
{
	/* pop ::= "pop" named_resource */

	int dst_size;
	char *src, *dst;

	if (!vm_next_named_resource(vm, vm_next(vm), &dst, &dst_size)) {
		err_push("VM", "Failed reading resource for pop operation");
		return false;
	}

	src = vm->stack.data + vm->stack_pointer - dst_size;
	memcpy(dst, src, dst_size);
	vm->stack_pointer -= dst_size;

	return true;
}

static bool mv_op_incr_decr(struct MoonVm *vm, int increment)
{
	int arg_size;
	char *arg;
	CELL_T resource_id = vm_next(vm);

	if (vm_next_resource_register(vm, resource_id, &arg, &arg_size) ||
		vm_next_resource_pointer(vm, resource_id, &arg, &arg_size)) {

		REG_T* value = (REG_T*)arg;
		assert(sizeof(ADDRESS_T) <= sizeof(REG_T));
		*value += increment;
		return true;

	} else {
		err_push("VM",
			"Increment or decrement operation on non register or pointer argument");
		return false;
	}
}

static bool vm_op_bin_arythmetic(struct MoonVm *vm, void(*op)(REG_T*, REG_T*))
{
	int x_size, y_size;
	char *x, *y;
	CELL_T x_res = vm_next(vm);
	CELL_T y_res = vm_next(vm);

	if ((vm_next_resource_register(vm, x_res, &x, &x_size) ||
			vm_next_resource_pointer(vm, x_res, &x, &x_size)) &&
		(vm_next_resource_register(vm, y_res, &y, &y_size) ||
			vm_next_resource_pointer(vm, y_res, &y, &y_size))) {
		assert(sizeof(ADDRESS_T) <= sizeof(REG_T));
		op((REG_T*)x, (REG_T*)y);
		return true;
	} else {
		err_push("VM",
			"Binary arythmetic operator can only be applied to registers or pointers");
		return false;
	}
}

static bool vm_op_bin_logic(struct MoonVm *vm, void(*op)(REG_T*, REG_T*))
{
	int x_size, y_size;
	char *x, *y;
	CELL_T x_res = vm_next(vm);
	CELL_T y_res = vm_next(vm);

	if ((vm_next_resource_register(vm, x_res, &x, &x_size) ||
			vm_next_resource_flag(vm, x_res, &x, &x_size) ||
			vm_next_resource_pointer(vm, x_res, &x, &x_size)) &&
		(vm_next_resource_register(vm, y_res, &y, &y_size) ||
			vm_next_resource_flag(vm, y_res, &y, &y_size) ||
			vm_next_resource_pointer(vm, y_res, &y, &y_size))) {

		assert(sizeof(ADDRESS_T) <= sizeof(REG_T));
		assert(sizeof(FLAG_T) <= sizeof(REG_T));

		REG_T x_temp = *((REG_T*)x);
		REG_T y_temp = *((REG_T*)y);

		op(&x_temp, &y_temp);

		if ((x_res > 0 && x_res < REG_COUNT) ||
			(x_res == MR_SP) || (x_res == MR_IP)) {
			*((REG_T*)x) = x_temp;
		} else {
			*((FLAG_T*)x) = (FLAG_T)x_temp;
		}

		return true;

	} else {
		err_push("VM",
			"Binary arythmetic operator can only be applied to registers or pointers");
		return false;
	}
}

static bool vm_op_un_arythmetic(struct MoonVm *vm, void(*op)(REG_T*))
{
	int x_size;
	char *x;

	if (vm_next_resource_register(vm, vm_next(vm), &x, &x_size)) {
		op((REG_T*)x);
		return true;
	} else {
		err_push("VM",
			"Unary arythmetic operator can only be applied to registers");
		return false;
	}
}

static bool vm_op_test(struct MoonVm *vm)
{

}

static bool vm_step(struct MoonVm *vm)
{
	switch (vm_next(vm)) {
	case MOP_PUT:
		return vm_op_put(vm);

	case MOP_PUSH:
		return vm_op_push(vm);

	case MOP_POP:
		return vm_op_pop(vm);

	case MOP_INCR:
		return mv_op_incr_decr(vm, 1);

	case MOP_DECR:
		return mv_op_incr_decr(vm, -1);

	case MOP_ADD:
		return vm_op_bin_arythmetic(vm, reg_ptr_add);

	case MOP_SUB:
		return vm_op_bin_arythmetic(vm, reg_ptr_sub);

	case MOP_MUL:
		return vm_op_bin_arythmetic(vm, reg_ptr_mul);

	case MOP_DIV:
		return vm_op_bin_arythmetic(vm, reg_ptr_div);

	case MOP_MOD:
		return vm_op_bin_arythmetic(vm, reg_ptr_mod);

	case MOP_AND:
		return vm_op_bin_logic(vm, reg_ptr_and);

	case MOP_OR:
		return vm_op_bin_logic(vm, reg_ptr_or);

	case MOP_XOR:
		return vm_op_bin_logic(vm, reg_ptr_xor);

	case MOP_NOT:
		return vm_op_un_arythmetic(vm, reg_ptr_not);

	case MOP_TEST:
		return vm_op_test(vm);

	default:
		err_push("VM", "Unknown operation code encountered");
		return false;
	}
}

bool vm_run(struct MoonVm *vm)
{
	err_reset();

	while (vm->stack.data[vm->instruction_pointer] != MOP_TERMINATE) {
		if (!vm_step(vm)) {
			err_push("VM", "Failed executing operation");
			return false;
		}
	}

	return true;
};
