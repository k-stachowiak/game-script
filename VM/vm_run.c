/* Copytight (C) 2015 Krzysztof Stachowiak */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vm.h"

#define MIN(x, y) ((x > y) ? y : x)

static bool vm_op_put(struct MoonVm *vm)
{
	/* put ::= "put" named_resource resource */

	int dst_size, src_size;
	char *dst, *src;

	if (!(vm_next_named_resource(vm, &dst, &dst_size) &&
		vm_next_resource(vm, &src, &src_size))) {
		return false;
	}

	memset(dst, 0, dst_size);
	memcpy(dst, src, MIN(src_size, dst_size));

	return true;
}

static bool vm_op_push(struct MoonVm *vm)
{
	/* push ::= push resource */

	int src_size;
	char *src, *dst;

	if (!vm_next_resource(vm, &src, &src_size)) {
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
	/* pop ::= pop named_resource */

	int dst_size;
	char *src, *dst;

	if (!vm_next_named_resource(vm, &dst, &dst_size)) {
		return false;
	}

	src = vm->stack.data + vm->stack_pointer - dst_size;
	memcpy(dst, src, dst_size);
	vm->stack_pointer -= dst_size;

	return true;
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

	default:
		fprintf(stderr, "Invalid operation encountered");
		return false;
	}

	return true;
}

bool vm_run(struct MoonVm *vm)
{
	while (vm->stack.data[vm->instruction_pointer] != MOP_TERMINATE) {
		if (!vm_step(vm)) {
			return false;
		}
	}

	return true;
};
