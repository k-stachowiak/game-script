/* Copyright (C) 2014 Krzysztof Stachowiak */

#include <string.h>

#include "rt_val.h"

/**
 * Advance location by:
 * - value header = VAL_HEAD_BYTES
 * - value size   = value.header.size
 */
VAL_LOC_T rt_val_next_loc(struct Runtime *rt, VAL_LOC_T loc)
{
    struct ValueHeader header = stack_peek_header(rt->stack, loc);
    return loc + VAL_HEAD_BYTES + header.size;
}

/** Peek the type of the value at the given location. */
enum ValueType rt_val_peek_type(struct Runtime *rt, VAL_LOC_T loc)
{
    struct ValueHeader header = stack_peek_header(rt->stack, loc);
    return (enum ValueType)header.type;
}

/** Peek the size of the value at the given location. */
VAL_SIZE_T rt_val_peek_size(struct Runtime *rt, VAL_LOC_T loc)
{
    struct ValueHeader header = stack_peek_header(rt->stack, loc);
    return header.size;
}

/** Peek a boolean value at the given location. */
VAL_BOOL_T rt_val_peek_bool(struct Runtime *rt, VAL_LOC_T loc)
{
    VAL_BOOL_T result;
    memcpy(&result, rt->stack->buffer + loc + VAL_HEAD_BYTES, VAL_BOOL_BYTES);
    return result;
}

/** Peek a character value at the given location. */
VAL_CHAR_T rt_val_peek_char(struct Runtime *rt, VAL_LOC_T loc)
{
    VAL_CHAR_T result;
    memcpy(&result, rt->stack->buffer + loc + VAL_HEAD_BYTES, VAL_CHAR_BYTES);
    return result;
}

/** Peek a integer value at the given location. */
VAL_INT_T rt_val_peek_int(struct Runtime *rt, VAL_LOC_T loc)
{
    VAL_INT_T result;
    memcpy(&result, rt->stack->buffer + loc + VAL_HEAD_BYTES, VAL_INT_BYTES);
    return result;
}

/** Peek a real value at the given location. */
VAL_REAL_T rt_val_peek_real(struct Runtime *rt, VAL_LOC_T loc)
{
    VAL_REAL_T result;
    memcpy(&result, rt->stack->buffer + loc + VAL_HEAD_BYTES, VAL_REAL_BYTES);
    return result;
}

/** Peek a string value at the given location. */
char* rt_val_peek_string(struct Runtime *rt, VAL_LOC_T loc)
{
    return rt->stack->buffer + loc + VAL_HEAD_BYTES;
}

/** Returns the location of the first element of the compound value. */
VAL_LOC_T rt_val_cpd_first_loc(VAL_LOC_T loc)
{
    return loc + VAL_HEAD_BYTES;
}

/** Computes the relevant locations of a function value. */
void rt_val_function_locs(
        struct Runtime *rt, 
        VAL_LOC_T loc,
        VAL_LOC_T *impl_loc,
        VAL_LOC_T *cap_start,
        VAL_LOC_T *appl_start)
{
    int cap_count, i;

	loc += VAL_HEAD_BYTES;

    *impl_loc = loc;
    loc += VAL_PTR_BYTES;

    *cap_start = loc;
    cap_count = rt_peek_size(rt, loc);

    loc += VAL_SIZE_BYTES;

    for (i = 0; i < cap_count; ++i) {
        loc = rt_val_fun_next_cap_loc(rt, loc);
    }

    *appl_start = loc;
}

char *rt_val_peek_fun_cap_symbol(struct Runtime *rt, VAL_LOC_T cap_loc)
{
    return rt->stack->buffer + VAL_SIZE_BYTES + cap_loc;
}

VAL_LOC_T rt_val_fun_cap_loc(struct Runtime *rt, VAL_LOC_T cap_loc)
{
	VAL_SIZE_T len;
	memcpy(&len, rt->stack->buffer + cap_loc, VAL_SIZE_BYTES);
    return cap_loc + VAL_SIZE_BYTES + len;
}

VAL_LOC_T rt_val_fun_next_cap_loc(struct Runtime *rt, VAL_LOC_T loc)
{
    return rt_val_next_loc(rt, rt_val_fun_cap_loc(rt, loc));
}

VAL_LOC_T rt_val_fun_next_appl_loc(struct Runtime *rt, VAL_LOC_T loc)
{
    return rt_val_next_loc(rt, loc);
}

