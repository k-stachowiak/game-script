/* Copyright (C) 2014,2015 Krzysztof Stachowiak */

#include "log.h"
#include "eval.h"
#include "bif.h"
#include "define.h"
#include "bif_detail.h"
#include "stack.h"
#include "runtime.h"
#include "rt_val.h"
#include "error.h"

#define BIF_LOGIC_UNARY_DEF(NAME) \
    void NAME(struct Runtime *rt, VAL_LOC_T x_loc) \
    { \
        enum ValueType x_type = rt_val_peek_type(rt, x_loc); \
        if (x_type != VAL_BOOL) { \
            bif_logic_error_arg_mismatch(); \
            return; \
        } \
        rt_val_push_bool(rt->stack, NAME##_impl(rt_val_peek_bool(rt, x_loc))); \
    }

#define BIF_LOGIC_BINARY_DEF(NAME) \
    void NAME(struct Runtime *rt, VAL_LOC_T x_loc, VAL_LOC_T y_loc) \
    { \
        enum ValueType x_type = rt_val_peek_type(rt, x_loc); \
        enum ValueType y_type = rt_val_peek_type(rt, y_loc); \
        if (x_type != VAL_BOOL || y_type != VAL_BOOL) { \
            bif_logic_error_arg_mismatch(); \
            return; \
        } \
        rt_val_push_bool(rt->stack, NAME##_impl( \
                rt_val_peek_bool(rt, x_loc), \
                rt_val_peek_bool(rt, y_loc))); \
    }

static void bif_logic_error_arg_mismatch(void)
{
    struct ErrMessage msg;
    err_msg_init_src(&msg, "EVAL BIF LOGIC", eval_location_top());
    err_msg_append(&msg, "Arguments of logic BIF must be of boolean type");
    err_msg_set(&msg);
}

// static VAL_BOOL_T bif_and_impl(VAL_BOOL_T x, VAL_BOOL_T y) { return (bool)x && (bool)y; }
// static VAL_BOOL_T bif_or_impl(VAL_BOOL_T x, VAL_BOOL_T y) { return (bool)x || (bool)y; }
static VAL_BOOL_T bif_xor_impl(VAL_BOOL_T x, VAL_BOOL_T y) { return (bool)x ^ (bool)y; }
static VAL_BOOL_T bif_not_impl(VAL_BOOL_T x) { return !!x; }

// BIF_LOGIC_BINARY_DEF(bif_and)
// BIF_LOGIC_BINARY_DEF(bif_or)
BIF_LOGIC_BINARY_DEF(bif_xor)
BIF_LOGIC_UNARY_DEF(bif_not)
