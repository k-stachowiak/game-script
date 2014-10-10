/* Copyright (C) 2014 Krzysztof Stachowiak */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "stack.h"

static VAL_HEAD_SIZE_T zero = 0;

struct Stack *stack_make(VAL_LOC_T size)
{
	struct Stack *result = malloc_or_die(sizeof(*result));
	result->buffer = malloc_or_die(size);
    result->size = size;
    result->top = 0;
    return result;
}

void stack_free(struct Stack *stack)
{
    free_or_die(stack->buffer);
    free_or_die(stack);
}

VAL_LOC_T stack_push(struct Stack *stack, VAL_LOC_T size, char *data)
{
    char *dst;

    if (size == 0) {
    	return stack->top;
    }

    if (stack->top + size >= stack->size) {
		LOG_ERROR("Stack overflow.\n");
		exit(1);
    }

    dst = stack->buffer + stack->top;
    memcpy(dst, data, size);
    stack->top += size;

	return stack->top - size;
}

void stack_push_copy(struct Stack *stack, VAL_LOC_T location)
{
	struct ValueHeader header = stack_peek_header(stack, location);
	stack_push(stack, header.size + VAL_HEAD_BYTES, stack->buffer + location);
}

void stack_collapse(struct Stack *stack, VAL_LOC_T begin, VAL_LOC_T end)
{
    VAL_LOC_T i;
	VAL_LOC_T removed = end - begin;
    VAL_LOC_T remaining = stack->top - end;
	for (i = 0; i < remaining; ++i) {
        *(stack->buffer + begin + i) = *(stack->buffer + end + i);
    }
	stack->top -= removed;
}

void stack_for_each(struct Stack *stack, void *state, void(*f)(void*, VAL_LOC_T))
{
	VAL_LOC_T loc = 0;
	while (loc != stack->top) {
		struct ValueHeader header = stack_peek_header(stack, loc);
		f(state, loc);
		loc += header.size + VAL_HEAD_BYTES;
	}
}

struct ValueHeader stack_peek_header(struct Stack *stack, VAL_LOC_T location)
{
    struct ValueHeader result;
    char *src = stack->buffer + location;
    memcpy(&result.type, src, VAL_HEAD_TYPE_BYTES);
    memcpy(&result.size, src + VAL_HEAD_TYPE_BYTES, VAL_HEAD_SIZE_BYTES);
    return result;
}

