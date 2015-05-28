/* Copyright (C) 2014,2015 Krzysztof Stachowiak */

#include <stdlib.h>
#include <string.h>

#include "memory.h"
#include "strbuild.h"
#include "log.h"
#include "runtime.h"
#include "rt_val.h"

VAL_HEAD_SIZE_T zero = 0;
VAL_HEAD_SIZE_T bool_size = VAL_BOOL_BYTES;
VAL_HEAD_SIZE_T char_size = VAL_CHAR_BYTES;
VAL_HEAD_SIZE_T int_size = VAL_INT_BYTES;
VAL_HEAD_SIZE_T real_size = VAL_REAL_BYTES;

static bool rt_val_pair_homo(struct Runtime *rt, VAL_LOC_T x, VAL_LOC_T y);

static bool rt_val_pair_homo_simple(struct Runtime *rt, VAL_LOC_T x, VAL_LOC_T y)
{
    struct ValueHeader
        header_x = rt_val_peek_header(&rt->stack, x),
        header_y = rt_val_peek_header(&rt->stack, y);

    return (
        (header_x.type == VAL_BOOL && header_y.type == VAL_BOOL) ||
        (header_x.type == VAL_CHAR && header_y.type == VAL_CHAR) ||
        (header_x.type == VAL_INT && header_y.type == VAL_INT) ||
        (header_x.type == VAL_REAL && header_y.type == VAL_REAL) ||
        (header_x.type == VAL_FUNCTION && header_y.type == VAL_FUNCTION)
    );
}

static bool rt_val_pair_homo_complex(struct Runtime *rt, VAL_LOC_T x, VAL_LOC_T y)
{
    VAL_LOC_T current_x, current_y;
    int i, len_x, len_y;

    struct ValueHeader
        header_x = rt_val_peek_header(&rt->stack, x),
        header_y = rt_val_peek_header(&rt->stack, y);

    if (header_x.type == VAL_ARRAY && header_y.type == VAL_ARRAY) {

        /* In case of arrays we only compare the first elements as the homogenity
         * is assured by the language rules in static and dynamic checks.
         * Note that empty array will type-match any other array.
         */

        len_x = rt_val_cpd_len(rt, x);
        len_y = rt_val_cpd_len(rt, y);

        if (len_x == 0 || len_y == 0) {
            return true;

        } else {
            current_x = rt_val_cpd_first_loc(x);
            current_y = rt_val_cpd_first_loc(y);
            return rt_val_pair_homo(rt, current_x, current_y);

        }

    } else if (header_x.type == VAL_TUPLE && header_y.type == VAL_TUPLE) {

        /* In case of tuples, we must compare the element counts as well as
         * all the contents element-wise.
         */

        len_x = rt_val_cpd_len(rt, x);
        len_y = rt_val_cpd_len(rt, y);

        if (len_x != len_y) {
            return false;

        } else {
            current_x = rt_val_cpd_first_loc(x);
            current_y = rt_val_cpd_first_loc(y);

            for (i = 0; i < len_x; ++i) {
                if (!rt_val_pair_homo(rt, current_x, current_y)) {
                    return false;
                }
                current_x = rt_val_next_loc(rt, current_x);
                current_y = rt_val_next_loc(rt, current_y);
            }

            return true;
        }

    } else {
        return false;

    }
}

static bool rt_val_pair_homo(struct Runtime *rt, VAL_LOC_T x, VAL_LOC_T y)
{
    return rt_val_pair_homo_simple(rt, x, y) ||
           rt_val_pair_homo_complex(rt, x, y);
}

bool rt_val_compound_homo(struct Runtime *rt, VAL_LOC_T val_loc)
{
    int i, len = rt_val_cpd_len(rt, val_loc);
    VAL_LOC_T first, current;

    if (len < 2) {
        return true;
    }

    first = rt_val_cpd_first_loc(val_loc);
    current = rt_val_next_loc(rt, first);

    for (i = 1; i < len; ++i) {
        if (!rt_val_pair_homo(rt, first, current)) {
            return false;
        }
        current = rt_val_next_loc(rt, current);
    }

    return true;
}

bool rt_val_eq_rec(struct Runtime *rt, VAL_LOC_T x, VAL_LOC_T y)
{
    enum ValueType xtype, ytype;
    int xlen, ylen;

    if (rt_val_peek_size(rt, x) != rt_val_peek_size(rt, y)) {
        return false;
    }

    xtype = rt_val_peek_type(rt, x);
    ytype = rt_val_peek_type(rt, y);

    if (xtype != ytype) {
        return false;
    }

    switch (xtype) {
    case VAL_BOOL:
        return rt_val_peek_bool(rt, x) == rt_val_peek_bool(rt, y);

    case VAL_CHAR:
        return rt_val_peek_char(rt, x) == rt_val_peek_char(rt, y);

    case VAL_INT:
        return rt_val_peek_int(rt, x) == rt_val_peek_int(rt, y);

    case VAL_REAL:
        return rt_val_peek_real(rt, x) == rt_val_peek_real(rt, y);

    case VAL_ARRAY:
    case VAL_TUPLE:
        xlen = rt_val_cpd_len(rt, x);
        ylen = rt_val_cpd_len(rt, y);
        if (xlen != ylen) {
            return false;
        }
        x = rt_val_cpd_first_loc(x);
        y = rt_val_cpd_first_loc(y);
        while (xlen) {
            if (!rt_val_eq_rec(rt, x, y)) {
                return false;
            }
            x = rt_val_next_loc(rt, x);
            y = rt_val_next_loc(rt, y);
            --xlen;
        }
        return true;

    case VAL_FUNCTION:
        return false;
    }

    LOG_ERROR("Cannot get here");
    exit(1);
}

bool rt_val_eq_bin(struct Runtime *rt, VAL_LOC_T x, VAL_LOC_T y)
{
    VAL_SIZE_T x_size = rt_val_peek_size(rt, x);
    VAL_SIZE_T y_size = rt_val_peek_size(rt, y);

    if (x_size != y_size) {
        return false;
    }

    return memcmp(
            rt->stack.buffer + x,
            rt->stack.buffer + y,
            x_size + VAL_HEAD_BYTES) == 0;
}

bool rt_val_string_eq(struct Runtime *rt, VAL_LOC_T loc, char *str)
{
    char *stack_str = rt_val_peek_cpd_as_string(rt, loc);
    bool result = strcmp(stack_str, str) == 0;
    mem_free(stack_str);
    return result;
}
