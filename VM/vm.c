/* Copytight (C) 2015 Krzysztof Stachowiak */

#include <string.h>

#include "collection.h"
#include "vm.h"

void vm_init(struct MoonVm *vm)
{
	memset(vm, 0, sizeof(*vm));
}

void vm_deinit(struct MoonVm *vm)
{
	ARRAY_FREE(vm->stack);
	memset(vm, 0, sizeof(*vm));
}

void vm_reset(struct MoonVm *vm)
{
	vm->stack.size = 0;
	vm->stack_pointer = 0;
	vm->instruction_pointer = 0;
}

void vm_load(struct MoonVm *vm, CELL_T *data, int size)
{
	vm->stack_pointer += size;
	while (size--) ARRAY_APPEND(vm->stack, *data++);
}

void vm_expand_stack(struct MoonVm *vm, int size)
{
	while (size--) ARRAY_APPEND(vm->stack, 0);
}