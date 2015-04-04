/* Copyright (C) 2014,2015 Krzysztof Stachowiak */

#include "bif.h"
#include "bif_detail.h"
#include "stack.h"
#include "runtime.h"
#include "rt_val.h"
#include "src_iter.h"

struct SourceLocation bif_location = { SRC_LOC_BIF, -1, -1 };

enum ValueType bif_match_un(
        struct Runtime *rt,
        VAL_LOC_T x_loc,
        VAL_INT_T *i,
        VAL_REAL_T *r)
{
    enum ValueType type = rt_val_peek_type(rt, x_loc);

    if (type == VAL_INT) {
        *i = rt_val_peek_int(rt, x_loc);
        return VAL_INT;

    } else if (type == VAL_REAL) {
        *r = rt_val_peek_real(rt, x_loc);
        return VAL_REAL;

    } else {
        return type;
    }
}

enum BifBinaryMatch bif_match_bin(
        struct Runtime *rt,
        VAL_LOC_T x_loc, VAL_LOC_T y_loc,
        VAL_INT_T *ix, VAL_INT_T *iy,
        VAL_REAL_T *rx, VAL_REAL_T *ry)
{
    enum ValueType x_type = rt_val_peek_type(rt, x_loc);
    enum ValueType y_type = rt_val_peek_type(rt, y_loc);

    if (x_type == VAL_INT) {
        *ix = rt_val_peek_int(rt, x_loc);
        switch (y_type) {
        case VAL_INT:
            *iy = rt_val_peek_int(rt, y_loc);
            return BBM_BOTH_INT;
        case VAL_REAL:
            *rx = (VAL_REAL_T)(*ix);
            *ry = rt_val_peek_real(rt, y_loc);
            return BBM_HETERO;
        default:
            return BBM_MISMATCH;
        }

    } else if (x_type == VAL_REAL) {
        *rx = rt_val_peek_real(rt, x_loc);
        switch (y_type) {
        case VAL_INT:
            *iy = rt_val_peek_int(rt, y_loc);
            *ry = (VAL_REAL_T)(*iy);
            return BBM_HETERO;
        case VAL_REAL:
            *ry = rt_val_peek_real(rt, y_loc);
            return BBM_BOTH_REAL;
        default:
            return BBM_MISMATCH;
        }

    } else {
        return BBM_MISMATCH;

    }
}

