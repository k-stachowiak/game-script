/* Copyright (C) 2014,2015 Krzysztof Stachowiak */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "log.h"
#include "stack.h"
#include "memory.h"
#include "rt_val.h"

struct Stack *stack_make(void)
{
    static int initial_size = 2;
    struct Stack *result = mem_malloc(sizeof(*result));
    result->buffer = mem_malloc(initial_size);
    result->size = initial_size;
    result->top = 1;
    return result;
}

void stack_free(struct Stack *stack)
{
    mem_free(stack->buffer);
    mem_free(stack);
}

VAL_LOC_T stack_push(struct Stack *stack, VAL_LOC_T size, char *data)
{
    char *dst;

    if (size == 0) {
        return stack->top;
    }

    while (stack->top + size >= stack->size) {
        VAL_LOC_T new_size = stack->size * 1.5;
        stack->buffer = mem_realloc(stack->buffer, new_size);
        stack->size = new_size;
    }

    dst = stack->buffer + stack->top;
    memcpy(dst, data, size);
    stack->top += size;

    return stack->top - size;
}

void stack_push_copy(struct Stack *stack, VAL_LOC_T location)
{
    struct ValueHeader header = rt_val_peek_header(stack, location);
    stack_push(stack, header.size + VAL_HEAD_BYTES, stack->buffer + location);
}

/** Peek a size_t at a given location */
VAL_SIZE_T stack_peek_size(struct Stack *stack, VAL_LOC_T loc)
{
    VAL_SIZE_T result;
    memcpy(&result, stack->buffer + loc, VAL_SIZE_BYTES);
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

void stack_for_each(struct Stack *stack, void *state, void(*f)(void*, VAL_LOC_T))
{
    VAL_LOC_T loc = 0;
    while (loc != stack->top) {
        struct ValueHeader header = rt_val_peek_header(stack, loc);
        f(state, loc);
        loc += header.size + VAL_HEAD_BYTES;
    }
}
