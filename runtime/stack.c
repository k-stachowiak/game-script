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

void stack_push_bool(struct Stack *stack, VAL_BOOL_T value)
{
	VAL_HEAD_TYPE_T type = (VAL_HEAD_TYPE_T)VAL_BOOL;
	VAL_HEAD_SIZE_T size = VAL_BOOL_BYTES;
	stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type);
	stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&size);
	stack_push(stack, size, (char*)&value);
}

void stack_push_char(struct Stack *stack, VAL_CHAR_T value)
{
	VAL_HEAD_TYPE_T type = (VAL_HEAD_TYPE_T)VAL_CHAR;
	VAL_HEAD_SIZE_T size = VAL_CHAR_BYTES;
	stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type);
	stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&size);
	stack_push(stack, size, (char*)&value);
}

void stack_push_int(struct Stack *stack, VAL_INT_T value)
{
	VAL_HEAD_TYPE_T type = (VAL_HEAD_TYPE_T)VAL_INT;
	VAL_HEAD_SIZE_T size = VAL_INT_BYTES;
	stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type);
	stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&size);
	stack_push(stack, size, (char*)&value);
}

void stack_push_real(struct Stack *stack, VAL_REAL_T value)
{
	VAL_HEAD_TYPE_T type = (VAL_HEAD_TYPE_T)VAL_REAL;
	VAL_HEAD_SIZE_T size = VAL_REAL_BYTES;
	stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type);
	stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&size);
	stack_push(stack, size, (char*)&value);
}

void stack_push_string(struct Stack *stack, char *value)
{
	VAL_HEAD_TYPE_T type = (VAL_HEAD_TYPE_T)VAL_STRING;
	VAL_HEAD_SIZE_T size = strlen(value) + 1;
	stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type);
	stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&size);
	stack_push(stack, size, value);
}

void stack_push_copy(struct Stack *stack, VAL_LOC_T location)
{
	struct ValueHeader header = stack_peek_header(stack, location);
	stack_push(stack, header.size + VAL_HEAD_BYTES, stack->buffer + location);
}

void stack_push_array_init(struct Stack *stack, VAL_LOC_T *size_loc)
{
	VAL_HEAD_TYPE_T type = (VAL_HEAD_TYPE_T)VAL_ARRAY;
	stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type);
	*size_loc = stack_push(stack, VAL_SIZE_BYTES, (char*)&zero);
}

void stack_push_tuple_init(struct Stack *stack, VAL_LOC_T *size_loc)
{
	VAL_HEAD_TYPE_T type = (VAL_HEAD_TYPE_T)VAL_TUPLE;
	stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type);
	*size_loc = stack_push(stack, VAL_SIZE_BYTES, (char*)&zero);
}

void stack_push_cpd_final(struct Stack *stack, VAL_LOC_T size_loc, VAL_SIZE_T size)
{
	memcpy(stack->buffer + size_loc, &size, VAL_HEAD_SIZE_BYTES);
}

void stack_push_func_init(
		struct Stack *stack,
		VAL_LOC_T *size_loc,
		VAL_LOC_T *data_begin,
		void *func_def)
{
	static VAL_HEAD_TYPE_T type = (VAL_HEAD_TYPE_T)VAL_FUNCTION;
	stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type);
	*size_loc = stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&zero);
	*data_begin = stack_push(stack, VAL_PTR_BYTES, (char*)&func_def);
}

void stack_push_func_cap_init_deferred(struct Stack *stack, VAL_LOC_T *cap_count_loc)
{
	*cap_count_loc = stack_push(stack, VAL_SIZE_BYTES, (char*)&zero);
}

void stack_push_func_cap_init(struct Stack *stack, VAL_SIZE_T cap_count)
{
	stack_push(stack, VAL_SIZE_BYTES, (char*)&cap_count);
}

void stack_push_func_cap(struct Stack *stack, char *symbol, VAL_LOC_T loc)
{
	VAL_SIZE_T len = strlen(symbol) + 1;
	stack_push(stack, VAL_SIZE_BYTES, (char*)&len);
	stack_push(stack, len + 1, symbol);
	stack_push_copy(stack, loc);
}

void stack_push_func_cap_copy(struct Stack *stack, VAL_LOC_T loc)
{
    char *symbol = stack->buffer + loc;
    VAL_SIZE_T len = strlen(symbol);
	stack_push(stack, VAL_SIZE_BYTES, (char*)&len);
	stack_push(stack, len + 1, symbol);
	stack_push_copy(stack, loc + len + 1);
}

void stack_push_func_cap_final_deferred(struct Stack *stack, VAL_LOC_T cap_count_loc, VAL_SIZE_T cap_count)
{
	memcpy(stack->buffer + cap_count_loc, &cap_count, VAL_SIZE_BYTES);
}

void stack_push_func_appl_init(struct Stack *stack, VAL_SIZE_T appl_count)
{
	stack_push(stack, VAL_SIZE_BYTES, (char*)&appl_count);
}

void stack_push_func_appl_empty(struct Stack *stack)
{
    stack_push(stack, VAL_SIZE_BYTES, (char*)&zero);
}

void stack_push_func_final(struct Stack *stack, VAL_LOC_T size_loc, VAL_SIZE_T data_begin)
{
	VAL_LOC_T data_size = stack->top - data_begin;
	memcpy(stack->buffer + size_loc, &data_size, VAL_HEAD_SIZE_BYTES);
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

struct ValueHeader stack_peek_header(struct Stack *stack, VAL_LOC_T location)
{
    struct ValueHeader result;
    char *src = stack->buffer + location;
    memcpy(&result.type, src, VAL_HEAD_TYPE_BYTES);
    memcpy(&result.size, src + VAL_HEAD_TYPE_BYTES, VAL_HEAD_SIZE_BYTES);
    return result;
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

