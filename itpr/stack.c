/* Copyright (C) 2014 Krzysztof Stachowiak */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "stack.h"
#include "value.h"

static VAL_HEAD_SIZE_T zero = 0;

struct Value stack_peek_value(struct Stack *stack, ptrdiff_t location);

static void stack_peek_compound(
        struct Stack *stack,
        struct Value *result,
        VAL_LOC_T loc)
{
    VAL_LOC_T location = loc + VAL_HEAD_BYTES;
    VAL_LOC_T end = loc + result->header.size + VAL_HEAD_BYTES;
    struct Value value;

    result->compound.data = NULL;
    result->compound.cap = 0;
    result->compound.size = 0;

    while (location != end) {
        value = stack_peek_value(stack, location);
        ARRAY_APPEND(result->compound, value);
        location += value.header.size + VAL_HEAD_BYTES;
    }
}

static void stack_peek_function(
        struct Stack *stack,
        struct Value *result,
        VAL_LOC_T loc)
{
    VAL_LOC_T location = loc + VAL_HEAD_BYTES;
    void *impl;
    VAL_SIZE_T i, size;

    /* Peek the implementation pointer. */
    memcpy(&impl, stack->buffer + location, VAL_PTR_BYTES);
    result->function.def = (struct AstNode*)impl;
    location += VAL_PTR_BYTES;

    /* Peek captures. */
    memcpy(&size, stack->buffer + location, VAL_SIZE_BYTES);
    result->function.captures.data = NULL;
    result->function.captures.size = 0;
    result->function.captures.cap = 0;
    location += VAL_SIZE_BYTES;

    for (i = 0; i < size; ++i) {
        VAL_SIZE_T len;
        struct Capture cap;
        struct ValueHeader header;

        memcpy(&len, stack->buffer + location, VAL_SIZE_BYTES);
        location += VAL_SIZE_BYTES;

		cap.symbol = malloc_or_die(len + 1);
        memcpy(cap.symbol, stack->buffer + location, len);
        cap.symbol[len] = '\0';
        location += len;

        cap.location = location;

        ARRAY_APPEND(result->function.captures, cap);

        header = stack_peek_header(stack, location);
        location += header.size + VAL_HEAD_BYTES;
    }

    /* Peek applied arguments. */
    memcpy(&size, stack->buffer + location, VAL_SIZE_BYTES);
    result->function.applied.data = NULL;
    result->function.applied.size = 0;
    result->function.applied.cap = 0;
    location += VAL_SIZE_BYTES;

    for (i = 0; i < size; ++i) {
        struct ValueHeader header;
        ARRAY_APPEND(result->function.applied, location);
        header = stack_peek_header(stack, location);
        location += header.size + VAL_HEAD_BYTES;
    }
}

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
    free(stack->buffer);
    free(stack);
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
	VAL_HEAD_TYPE_T type = (VAL_HEAD_TYPE_T)VAL_BOOL;
	VAL_HEAD_SIZE_T size = strlen(value);
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
	VAL_SIZE_T len = strlen(symbol);
	struct ValueHeader header = stack_peek_header(stack, loc);
	stack_push(stack, VAL_SIZE_BYTES, (char*)&len);
	stack_push(stack, len, symbol);
	stack_push(stack, header.size + VAL_HEAD_BYTES, stack->buffer + loc);
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

struct Value stack_peek_value(struct Stack *stack, VAL_LOC_T location)
{
    char *src = stack->buffer + location;
    struct Value result;

    result.header = stack_peek_header(stack, location);

    switch ((enum ValueType)result.header.type) {
    case VAL_BOOL:
        memcpy(
            &(result.primitive.boolean),
            src + VAL_HEAD_BYTES,
            VAL_BOOL_BYTES);
        break;

    case VAL_CHAR:
        memcpy(
            &(result.primitive.character),
            src + VAL_HEAD_BYTES,
            VAL_CHAR_BYTES);
        break;

    case VAL_INT:
        memcpy(
            &(result.primitive.integer),
            src + VAL_HEAD_BYTES,
            VAL_INT_BYTES);
        break;

    case VAL_REAL:
        memcpy(
            &(result.primitive.real),
            src + VAL_HEAD_BYTES,
            VAL_REAL_BYTES);
        break;

    case VAL_STRING:
        result.string.str_begin = src + VAL_HEAD_BYTES;
        result.string.str_len = result.header.size;
        break;

    case VAL_ARRAY:
    case VAL_TUPLE:
        stack_peek_compound(stack, &result, location);
        break;

    case VAL_FUNCTION:
        stack_peek_function(stack, &result, location);
        break;

    default:
		LOG_ERROR("Unhandled value type.\n");
        exit(1);
    }

    return result;
}

void stack_for_each(
		struct Stack *stack,
		void(*f)(VAL_LOC_T, struct Value*))
{
	VAL_LOC_T loc = 0;
	while (loc != stack->top) {
		struct Value val = stack_peek_value(stack, loc);
		f(loc, &val);
		loc += val.header.size + VAL_HEAD_BYTES;
	}
}
