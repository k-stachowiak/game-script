/* Copyright (C) 2015 Krzysztof Stachowiak */

#include <inttypes.h>
#include <string.h>

#include "log.h"
#include "rt_val.h"
#include "stack.h"
#include "memory.h"

void rt_val_push_copy(struct Stack *stack, VAL_LOC_T location)
{
    struct ValueHeader header = rt_val_peek_header(stack, location);

    VAL_LOC_T size = header.size + VAL_HEAD_BYTES;

    char *temp_buffer = mem_malloc(size);
    memcpy(temp_buffer, stack->buffer + location, size);

    stack_push(stack, size, temp_buffer);

    mem_free(temp_buffer);
}

void rt_val_push_bool(struct Stack *stack, VAL_BOOL_T value)
{
    VAL_HEAD_TYPE_T type = (VAL_HEAD_TYPE_T)VAL_BOOL;
    VAL_BOOL_T normalized_value = !!value;
    stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type);
    stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&bool_size);
    stack_push(stack, bool_size, (char*)&normalized_value);
}

void rt_val_push_char(struct Stack *stack, VAL_CHAR_T value)
{
    VAL_HEAD_TYPE_T type = (VAL_HEAD_TYPE_T)VAL_CHAR;
    stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type);
    stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&char_size);
    stack_push(stack, char_size, (char*)&value);
}

void rt_val_push_int(struct Stack *stack, VAL_INT_T value)
{
    VAL_HEAD_TYPE_T type = (VAL_HEAD_TYPE_T)VAL_INT;
    stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type);
    stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&int_size);
    stack_push(stack, int_size, (char*)&value);
}

void rt_val_push_real(struct Stack *stack, VAL_REAL_T value)
{
    VAL_HEAD_TYPE_T type = (VAL_HEAD_TYPE_T)VAL_REAL;
    stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type);
    stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&real_size);
    stack_push(stack, real_size, (char*)&value);
}

void rt_val_push_ref(struct Stack *stack, VAL_REF_T value)
{
    VAL_HEAD_TYPE_T type = (VAL_HEAD_TYPE_T)VAL_REF;
    stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type);
    stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&ref_size);
    stack_push(stack, ref_size, (char*)&value);
}

void rt_val_push_unit(struct Stack *stack)
{
    VAL_HEAD_TYPE_T type = (VAL_HEAD_TYPE_T)VAL_UNIT;
    stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type);
    stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&unit_size);
}

void rt_val_push_array_init(struct Stack *stack, VAL_LOC_T *size_loc)
{
    VAL_HEAD_TYPE_T type = (VAL_HEAD_TYPE_T)VAL_ARRAY;
    stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type);
    *size_loc = stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&zero);
}

void rt_val_push_tuple_init(struct Stack *stack, VAL_LOC_T *size_loc)
{
    VAL_HEAD_TYPE_T type = (VAL_HEAD_TYPE_T)VAL_TUPLE;
    stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type);
    *size_loc = stack_push(stack, VAL_SIZE_BYTES, (char*)&zero);
}

void rt_val_push_cpd_final(
struct Stack *stack,
    VAL_LOC_T size_loc,
    VAL_SIZE_T size)
{
    memcpy(stack->buffer + size_loc, &size, VAL_HEAD_SIZE_BYTES);
}

void rt_val_push_string(struct Stack *stack, char *begin, char *end)
{
    VAL_LOC_T data_begin, size_loc;
    rt_val_push_array_init(stack, &size_loc);
    data_begin = stack->top;
    while (begin != end) {
        rt_val_push_char(stack, *begin++);
    }
    rt_val_push_cpd_final(stack, size_loc, stack->top - data_begin);
}

void rt_val_push_func_init(
struct Stack *stack,
    VAL_LOC_T *size_loc,
    VAL_LOC_T *data_begin,
    VAL_SIZE_T arity,
enum ValueFuncType func_type,
    void *impl)
{
    static VAL_HEAD_TYPE_T type = (VAL_HEAD_TYPE_T)VAL_FUNCTION;
    stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type);
    *size_loc = stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&zero);
    *data_begin = stack_push(stack, VAL_SIZE_BYTES, (char*)&arity);
    stack_push(stack, VAL_TYPE_BYTES, (char*)&func_type);
    stack_push(stack, VAL_PTR_BYTES, (char*)&impl);
}

void rt_val_push_func_cap_init_deferred(
struct Stack *stack,
    VAL_LOC_T *cap_count_loc)
{
    *cap_count_loc = stack_push(stack, VAL_SIZE_BYTES, (char*)&zero);
}

void rt_val_push_func_cap_init(struct Stack *stack, VAL_SIZE_T cap_count)
{
    stack_push(stack, VAL_SIZE_BYTES, (char*)&cap_count);
}

void rt_val_push_func_cap(struct Stack *stack, char *symbol, VAL_LOC_T loc)
{
    VAL_SIZE_T len = (VAL_SIZE_T)strlen(symbol) + 1;
    stack_push(stack, VAL_SIZE_BYTES, (char*)&len);
    stack_push(stack, len, symbol);
    rt_val_push_copy(stack, loc);
}

void rt_val_push_func_cap_copy(struct Stack *stack, VAL_LOC_T loc)
{
    char *symbol = stack->buffer + loc;
    VAL_SIZE_T len = (VAL_SIZE_T)strlen(symbol);
    stack_push(stack, VAL_SIZE_BYTES, (char*)&len);
    stack_push(stack, len + 1, symbol);
    rt_val_push_copy(stack, loc + len + 1);
}

void rt_val_push_func_cap_final_deferred(
struct Stack *stack,
    VAL_LOC_T cap_count_loc,
    VAL_SIZE_T cap_count)
{
    memcpy(stack->buffer + cap_count_loc, &cap_count, VAL_SIZE_BYTES);
}

void rt_val_push_func_appl_init(struct Stack *stack, VAL_SIZE_T appl_count)
{
    stack_push(stack, VAL_SIZE_BYTES, (char*)&appl_count);
}

void rt_val_push_func_final(
struct Stack *stack,
    VAL_LOC_T size_loc,
    VAL_SIZE_T data_begin)
{
    VAL_LOC_T data_size = stack->top - data_begin;
    memcpy(stack->buffer + size_loc, &data_size, VAL_HEAD_SIZE_BYTES);
}
