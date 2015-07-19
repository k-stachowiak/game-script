/* Copytight (C) 2015 Krzysztof Stachowiak */

#include <stdio.h>

#include "error.h"
#include "vm.h"

static bool vm_next_size_descriptor(
		struct MoonVm *vm,
		int *size)
{
	CELL_T size_id = vm_next(vm);
	switch (size_id) {
	case MR_1B:
		*size = 1;
		return true;

	case MR_2B:
		*size = 2;
		return true;

	case MR_4B:
		*size = 4;
		return true;

	case MR_8B:
		*size = 8;
		return true;

	default:
		err_push("VM", "Expected size specifier while reading resource");
		return false;
	}
}

CELL_T vm_next(struct MoonVm *vm)
{
	return vm->stack.data[vm->instruction_pointer++];
}

void vm_next_n(struct MoonVm *vm, CELL_T* dst, int n)
{
	while (n--) *dst++ = vm_next(vm);
}

bool vm_next_resource_register(
		struct MoonVm *vm,
		CELL_T res,
		CELL_T **addr,
		int *size)
{
	if (res >= 0 && res < REG_COUNT) {
		*addr = (CELL_T*)(vm->registers + res);
		*size = sizeof(REG_T);
		return true;
	} else {
		return false;
	}
}

bool vm_next_resource_flag(
		struct MoonVm *vm,
		CELL_T res,
		CELL_T **addr,
		int *size)
{
	switch (res) {
	case MR_ZF:
		*addr = (CELL_T*)(&vm->zero_flag);
		*size = sizeof(FLAG_T);
		return true;

	case MR_SF:
		*addr = (CELL_T*)(&vm->sign_flag);
		*size = sizeof(FLAG_T);
		return true;

	case MR_OF:
		*addr = (CELL_T*)(&vm->overflow_flag);
		*size = sizeof(FLAG_T);
		return true;

	case MR_PF:
		*addr = (CELL_T*)(&vm->parity_flag);
		*size = sizeof(FLAG_T);
		return true;

	case MR_CF:
		*addr = (CELL_T*)(&vm->carry_flag);
		*size = sizeof(FLAG_T);
		return true;
	}

	return false;
}

bool vm_next_resource_pointer(
		struct MoonVm *vm,
		CELL_T res,
		CELL_T **addr,
		int *size)
{
	switch (res) {
	case MR_IP:
		*addr = (CELL_T*)(&vm->instruction_pointer);
		*size = sizeof(ADDRESS_T);
		return true;

	case MR_SP:
		*addr = (CELL_T*)(&vm->stack_pointer);
		*size = sizeof(ADDRESS_T);
		return true;
	}

	return false;
}

bool vm_next_resource_local(
		struct MoonVm *vm,
		CELL_T res,
		CELL_T **addr,
		int *size)
{
	return
		vm_next_resource_register(vm, res, addr, size) ||
		vm_next_resource_flag(vm, res, addr, size) ||
		vm_next_resource_pointer(vm, res, addr, size);
}

bool vm_next_resource_literal(
		struct MoonVm *vm,
		CELL_T res,
		CELL_T **addr,
		int *size)
{
	if (res == MR_LITRL && vm_next_size_descriptor(vm, size)) {
		*addr = vm->stack.data + vm->instruction_pointer;
		vm->instruction_pointer += *size;
		return true;
	} else {
		return false;
	}
}

bool vm_next_resource_dereference(
		struct MoonVm *vm,
		CELL_T res,
		CELL_T **addr,
		int *size)
{
	ADDRESS_T address;
	if (res == MR_DEREF && vm_next_size_descriptor(vm, size)) {
		vm_next_n(vm, (CELL_T*)(&address), sizeof(address));
		*addr = vm->stack.data + address;
		return true;
	} else {
		return false;
	}
}

bool vm_next_resource(struct MoonVm *vm, CELL_T res, CELL_T **addr, int *size)
{
	return
		vm_next_resource_local(vm, res, addr, size) ||
		vm_next_resource_literal(vm, res, addr, size) ||
		vm_next_resource_dereference(vm, res, addr, size);
}

bool vm_next_named_resource(struct MoonVm *vm, CELL_T res, CELL_T **addr, int *size)
{
	return
		vm_next_resource_local(vm, res, addr, size) ||
		vm_next_resource_dereference(vm, res, addr, size);
}
