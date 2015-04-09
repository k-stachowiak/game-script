/* Copyright (C) 2014,2015 Krzysztof Stachowiak */

#include "log.h"
#include "eval.h"
#include "bif.h"
#include "bif_detail.h"
#include "stack.h"
#include "error.h"
#include "rt_val.h"

#define BIF_COMPARE_DEF(NAME) \
    void NAME(struct Runtime *rt, VAL_LOC_T x_loc, VAL_LOC_T y_loc) \
    { \
        VAL_REAL_T rx, ry; \
        VAL_INT_T ix, iy; \
        switch (bif_match_bin(rt, x_loc, y_loc, &ix, &iy, &rx, &ry)) { \
        case BBM_BOTH_INT: \
            rt_val_push_bool(rt->stack, NAME##_impl_int(ix, iy)); \
            break; \
        case BBM_BOTH_REAL: \
        case BBM_HETERO: \
            rt_val_push_bool(rt->stack, NAME##_impl_real(rx, ry)); \
            break; \
        case BBM_MISMATCH: \
            bif_compare_error_arg_mismatch(); \
            return; \
        } \
    }

static void bif_compare_error_arg_mismatch(void)
{
    struct ErrMessage msg;
    err_msg_init_src(&msg, "EVAL BIF COMPARE", eval_location_top());
    err_msg_append(&msg, "Arguments of comparison BIF must be of equal numeric type");
    err_msg_set(&msg);
}

static VAL_BOOL_T bif_lt_impl_int(VAL_INT_T x, VAL_INT_T y) { return x < y; }
static VAL_BOOL_T bif_gt_impl_int(VAL_INT_T x, VAL_INT_T y) { return x > y; }
static VAL_BOOL_T bif_leq_impl_int(VAL_INT_T x, VAL_INT_T y) { return x <= y; }
static VAL_BOOL_T bif_geq_impl_int(VAL_INT_T x, VAL_INT_T y) { return x >= y; }
static VAL_BOOL_T bif_lt_impl_real(VAL_REAL_T x, VAL_REAL_T y) { return x < y; }
static VAL_BOOL_T bif_gt_impl_real(VAL_REAL_T x, VAL_REAL_T y) { return x > y; }
static VAL_BOOL_T bif_leq_impl_real(VAL_REAL_T x, VAL_REAL_T y) { return x <= y; }
static VAL_BOOL_T bif_geq_impl_real(VAL_REAL_T x, VAL_REAL_T y) { return x >= y; }

BIF_COMPARE_DEF(bif_lt)
BIF_COMPARE_DEF(bif_gt)
BIF_COMPARE_DEF(bif_leq)
BIF_COMPARE_DEF(bif_geq)

void bif_eq(struct Runtime *rt, VAL_LOC_T x_loc, VAL_LOC_T y_loc)
{
    VAL_SIZE_T x_size = rt_val_peek_size(rt, x_loc);
    VAL_SIZE_T y_size = rt_val_peek_size(rt, y_loc);

    if (x_size != y_size) {
        rt_val_push_bool(rt->stack, false);
        return;
    }

    bool result = (memcmp(
            rt->stack->buffer + x_loc,
            rt->stack->buffer + y_loc,
            x_size + VAL_HEAD_BYTES) == 0);

    rt_val_push_bool(rt->stack, result);
}

