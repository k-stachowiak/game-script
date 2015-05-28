/* Copyright (C) 2015 Krzysztof Stachowiak */

#include <string.h>

#include "rt_val.h"
#include "stack.h"

void rt_val_poke_bool(struct Stack *stack, VAL_LOC_T loc, VAL_BOOL_T value)
{
	memcpy(stack->buffer + loc + VAL_HEAD_BYTES,
		(char*)&value,
		VAL_BOOL_BYTES);
}
