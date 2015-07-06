
/* Copyright (C) 2014,2015 Krzysztof Stachowiak */

#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "log.h"
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
VAL_HEAD_SIZE_T ref_size = VAL_REF_BYTES;
VAL_HEAD_SIZE_T unit_size = VAL_UNIT_BYTES;

bool rt_val_pair_homo(struct Runtime *rt, VAL_LOC_T x, VAL_LOC_T y)
{
    struct ValueHeader
        header_x = rt_val_peek_header(&rt->stack, x),
        header_y = rt_val_peek_header(&rt->stack, y);

    if ((header_x.type == VAL_ARRAY && header_y.type == VAL_ARRAY) ||
        (header_x.type == VAL_TUPLE && header_y.type == VAL_TUPLE)) {

		VAL_LOC_T current_x, current_y;
		int i, len_x, len_y;

        len_x = rt_val_cpd_len(rt, x);
        len_y = rt_val_cpd_len(rt, y);
        if (len_x != len_y) {
            return false;
        }

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

    } else {
        return header_x.type == header_y.type;

    }
}

bool rt_val_eq_rec(struct Runtime *rt, VAL_LOC_T x, VAL_LOC_T y)
{
    enum ValueType xtype, ytype;
    int xlen, ylen;

    if (rt_val_peek_size(&rt->stack, x) != rt_val_peek_size(&rt->stack, y)) {
        return false;
    }

    xtype = rt_val_peek_type(&rt->stack, x);
    ytype = rt_val_peek_type(&rt->stack, y);

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

	case VAL_REF:
		return rt_val_eq_rec(rt, rt_val_peek_ref(rt, x), rt_val_peek_ref(rt, y));

    case VAL_UNIT:
        return true;
    }

    LOG_ERROR("Cannot get here");
    exit(1);
}

bool rt_val_eq_bin(struct Runtime *rt, VAL_LOC_T x, VAL_LOC_T y)
{
    VAL_SIZE_T x_size = rt_val_peek_size(&rt->stack, x);
    VAL_SIZE_T y_size = rt_val_peek_size(&rt->stack, y);

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

