/* Copyright (C) 2014 Krzysztof Stachowiak */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "stack.h"
#include "value.h"

struct Value stack_peek_value(struct Stack *stack, ptrdiff_t location);

static void stack_peek_compound(struct Stack *stack, struct Value *result)
{
    ptrdiff_t location = result->begin + VAL_HEAD_BYTES;
    ptrdiff_t end = result->begin + result->header.size + VAL_HEAD_BYTES;
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

static void stack_peek_function(struct Stack *stack, struct Value *result)
{
    ptrdiff_t location = result->begin + VAL_HEAD_BYTES;
    void *impl;
    uint32_t i, size;

    memcpy(&impl, stack->buffer + location, VAL_PTR_BYTES);
    result->function.def = (struct AstNode*)impl;
    location += VAL_PTR_BYTES;

    memcpy(&size, stack->buffer + location, VAL_SIZE_BYTES);
    result->function.captures.data = NULL;
    result->function.captures.size = 0;
    result->function.captures.cap = 0;
    location += VAL_SIZE_BYTES;

    for (i = 0; i < size; ++i) {
        /* Peek captures. */
        exit(3);
    }

    memcpy(&size, stack->buffer + location, VAL_SIZE_BYTES);
    result->function.applied.data = NULL;
    result->function.applied.size = 0;
    result->function.applied.cap = 0;
    location += VAL_SIZE_BYTES;

    for (i = 0; i < size; ++i) {
        /* Peek applied arguments. */
        exit(3);
    }
}

struct Stack *stack_make(ptrdiff_t size)
{
    struct Stack *result = malloc(sizeof(*result));
    result->buffer = malloc(size);
    result->size = size;
    result->top = 0;
    return result;
}

void stack_free(struct Stack *stack)
{
    free(stack->buffer);
    free(stack);
}

void stack_push(struct Stack *stack, ptrdiff_t size, char *data)
{
    char *dst;

    if (stack->top + size >= stack->size) {
        /* TODO: Implement unified runtime error handling (OVERFLOW). */
        printf("Stack overflow.\n");
        exit(1);
    }

    dst = stack->buffer + stack->top;
    memcpy(dst, data, size);
    stack->top += size;
}

void stack_collapse(struct Stack *stack, ptrdiff_t begin, ptrdiff_t end)
{
    ptrdiff_t i;
    ptrdiff_t count = end - begin;
    for (i = 0; i < count; ++i) {
        *(stack->buffer + begin + i) = *(stack->buffer + end + i);
    }
    stack->top -= count;
}

struct ValueHeader stack_peek_header(struct Stack *stack, ptrdiff_t location)
{
    struct ValueHeader result;
    char *src = stack->buffer + location;
    memcpy(&result.type, src, VAL_HEAD_TYPE_BYTES);
    memcpy(&result.size, src + VAL_HEAD_TYPE_BYTES, VAL_HEAD_SIZE_BYTES);
    return result;
}

struct Value stack_peek_value(struct Stack *stack, ptrdiff_t location)
{
    char *src = stack->buffer + location;
    struct Value result;

    /* TODO: Consider completely untangling value from the stack. */
    result.begin = location;
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
        /* TODO: perform a copy here. */
        result.string.str_begin = src + VAL_HEAD_BYTES;
        result.string.str_len = result.header.size;
        break;

    case VAL_ARRAY:
    case VAL_TUPLE:
        stack_peek_compound(stack, &result);
        break;

    case VAL_FUNCTION:
        stack_peek_function(stack, &result);
        break;

    default:
        printf("Unhandled value type.\n");
        exit(1);
    }

    return result;
}