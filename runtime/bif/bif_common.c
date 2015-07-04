/* Copyright (C) 2014,2015 Krzysztof Stachowiak */

#include "bif.h"
#include "bif_detail.h"
#include "stack.h"
#include "runtime.h"
#include "rt_val.h"
#include "src_iter.h"

enum ValueType bif_match_un(
        struct Runtime *rt,
        VAL_LOC_T x_loc,
        VAL_BOOL_T *b,
        VAL_INT_T *i,
        VAL_REAL_T *r,
        VAL_CHAR_T *c)
{
    enum ValueType type = rt_val_peek_type(&rt->stack, x_loc);

    switch (type) {
    case VAL_BOOL:
        *b = rt_val_peek_bool(rt, x_loc);
        break;

    case VAL_INT:
        *i = rt_val_peek_int(rt, x_loc);
        break;

    case VAL_REAL:
        *r = rt_val_peek_real(rt, x_loc);
        break;

    case VAL_CHAR:
        *c = rt_val_peek_char(rt, x_loc);
        break;

    default:
        break;
    }

    return type;
}

enum BifBinaryMatch bif_match_bin(
        struct Runtime *rt,
        VAL_LOC_T x_loc, VAL_LOC_T y_loc,
        VAL_BOOL_T *bx, VAL_BOOL_T *by,
        VAL_INT_T *ix, VAL_INT_T *iy,
        VAL_REAL_T *rx, VAL_REAL_T *ry,
        VAL_CHAR_T *cx, VAL_CHAR_T *cy)
{
    enum ValueType x_type = rt_val_peek_type(&rt->stack, x_loc);
    enum ValueType y_type = rt_val_peek_type(&rt->stack, y_loc);

    if (x_type == VAL_BOOL && y_type == VAL_BOOL) {
        *bx = rt_val_peek_bool(rt, x_loc);
        *by = rt_val_peek_bool(rt, y_loc);
        return BBM_BOTH_BOOL;
    }

    if (x_type == VAL_INT && y_type == VAL_INT) {
        *ix = rt_val_peek_int(rt, x_loc);
        *iy = rt_val_peek_int(rt, y_loc);
        return BBM_BOTH_INT;
    }

    if (x_type == VAL_REAL && y_type == VAL_REAL) {
        *rx = rt_val_peek_real(rt, x_loc);
        *ry = rt_val_peek_real(rt, y_loc);
        return BBM_BOTH_REAL;
    }

    if (x_type == VAL_CHAR && y_type == VAL_CHAR) {
        *cx = rt_val_peek_char(rt, x_loc);
        *cy = rt_val_peek_char(rt, y_loc);
        return BBM_BOTH_CHAR;
    }

    if (x_type == VAL_INT && y_type == VAL_REAL) {
        *ix = rt_val_peek_int(rt, x_loc);
        *rx = (VAL_REAL_T)(*ix);
        *ry = rt_val_peek_real(rt, y_loc);
        return BBM_BOTH_NUMERIC;
    }

    if (x_type == VAL_REAL && y_type == VAL_INT) {
        *rx = rt_val_peek_real(rt, x_loc);
        *iy = rt_val_peek_int(rt, y_loc);
        *ry = (VAL_REAL_T)(*iy);
        return BBM_BOTH_NUMERIC;
    }

    return BBM_MISMATCH;
}

