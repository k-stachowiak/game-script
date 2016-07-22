/* Copyright (C) 2014-2016 Krzysztof Stachowiak */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "log.h"
#include "stack.h"
#include "memory.h"
#include "rt_val.h"

void stack_init(struct Stack *stack)
{
    static int initial_size = 2;
    stack->buffer = mem_malloc(initial_size);
    stack->size = initial_size;
    stack->top = 1;
}

void stack_deinit(struct Stack *stack)
{
    mem_free(stack->buffer);
}

VAL_LOC_T stack_push(struct Stack *stack, VAL_LOC_T size, char *data)
{
    char *dst;

    if (size == 0) {
        return stack->top;
    }

    while (stack->top + size >= stack->size) {
        VAL_LOC_T new_size = (VAL_LOC_T)(stack->size * 1.5);
        stack->buffer = mem_realloc(stack->buffer, new_size);
        stack->size = new_size;
    }

    dst = stack->buffer + stack->top;
    memcpy(dst, data, size);
    stack->top += size;

    return stack->top - size;
}

/** Peek a size_t at a given location */
VAL_SIZE_T stack_peek_size(struct Stack *stack, VAL_LOC_T loc)
{
    VAL_SIZE_T result;
    memcpy(&result, stack->buffer + loc, VAL_SIZE_BYTES);
    return result;
}

/** Peek a type_t at a given location */
VAL_TYPE_T stack_peek_type(struct Stack *stack, VAL_LOC_T loc)
{
    VAL_TYPE_T result;
    memcpy(&result, stack->buffer + loc, VAL_TYPE_BYTES);
    return result;
}

/** Peek a ptr_t at a given location */
void *stack_peek_ptr(struct Stack *stack, VAL_LOC_T loc)
{
    void *result;
    memcpy((char*)&result, stack->buffer + loc, VAL_PTR_BYTES);
    return result;
}

void stack_collapse(struct Stack *stack, VAL_LOC_T begin, VAL_LOC_T end)
{
    VAL_LOC_T removed = end - begin;
    VAL_LOC_T remaining = stack->top - end;
    memmove(stack->buffer + begin, stack->buffer + end, remaining);
    stack->top -= removed;
}

