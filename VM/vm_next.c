/* Copytight (C) 2015 Krzysztof Stachowiak */

#include <stdio.h>

#include "error.h"
#include "vm.h"

CELL_T vm_next(struct MoonVm *vm)
{
	return vm->stack.data[vm->instruction_pointer++];
}

void vm_next_n(struct MoonVm *vm, CELL_T* dst, int n)
{
	while (n--) *dst++ = vm_next(vm);
}

static bool vm_next_resource_local(
		struct MoonVm *vm,
		CELL_T resource_id,
		char **addr,
		int *size)
{
	/* Registers: */
	if (resource_id < REG_COUNT) {
		*addr = (char*)(vm->registers + resource_id);
		*size = sizeof(REG_T);
		return true;
	}

	switch (resource_id) {

	/* Flags: */
	case REG_COUNT + 0:
		*addr = (char*)(&vm->zero_flag);
		*size = sizeof(FLAG_T);
		return true;

	case REG_COUNT + 1:
		*addr = (char*)(&vm->sign_flag);
		*size = sizeof(FLAG_T);
		return true;

	case REG_COUNT + 2:
		*addr = (char*)(&vm->overflow_flag);
		*size = sizeof(FLAG_T);
		return true;

	/* Pointers: */
	case REG_COUNT + 3:
		*addr = (char*)(&vm->instruction_pointer);
		*size = sizeof(ADDRESS_T);
		return true;

	case REG_COUNT + 4:
		*addr = (char*)(&vm->stack_pointer);
		*size = sizeof(ADDRESS_T);
		return true;
	}

	return false;
}

static bool vm_next_resource_nonlocal(
		struct MoonVm *vm,
		CELL_T resource_id,
		char **addr,
		int *size)
{
	ADDRESS_T address;
	CELL_T size_id = vm_next(vm);

	switch (size_id) {
	case MR_1B:
		*size = 1;
		break;

	case MR_2B:
		*size = 2;
		break;

	case MR_4B:
		*size = 4;
		break;

	case MR_8B:
		*size = 8;
		break;

	default:
		err_push("VM", "Expected size specifier while reading resource");
		return false;
	}

	switch (resource_id) {
	case MR_LITRL:
		*addr = vm->stack.data + vm->instruction_pointer;
		vm->instruction_pointer += *size;
		break;

	case MR_DEREF:
		vm_next_n(vm, (char*)(&address), sizeof(address));
		*addr = vm->stack.data + address;
		break;

	default:
		err_push("VM", "Expected type specifier while reading resource");
		return false;
	}

	return true;
}

static bool vm_next_resource_impl(
		struct MoonVm *vm,
		CELL_T resource_id,
		char **addr,
		int *size)
{
	return
		vm_next_resource_local(vm, resource_id, addr, size) ||
		vm_next_resource_nonlocal(vm, resource_id, addr, size);
}

bool vm_next_resource(struct MoonVm *vm, char **addr, int *size)
{
	return vm_next_resource_impl(vm, vm_next(vm), addr, size);
}

bool vm_next_named_resource(struct MoonVm *vm, char **addr, int *size)
{
	CELL_T resource_id = vm_next(vm);
	if (resource_id == MR_LITRL) {
		err_push("VM", "Expected named resource but literal encountered");
		return false;
	}
	else {
		return vm_next_resource_impl(vm, resource_id, addr, size);
	}
}
