/* Copyright (C) 2014,2015 Krzysztof Stachowiak */

#include "log.h"
#include "eval.h"
#include "bif.h"
#include "bif_detail.h"
#include "stack.h"
#include "error.h"
#include "rt_val.h"

static void bif_compare_error_arg_mismatch(void)
{
    struct ErrMessage msg;
    err_msg_init_src(&msg, "EVAL BIF COMPARE", eval_location_top());
    err_msg_append(&msg, "Arguments of comparison BIF must be of equal numeric type");
    err_msg_set(&msg);
}

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


void bif_lt(struct Runtime *rt, VAL_LOC_T x_loc, VAL_LOC_T y_loc)
{
	VAL_REAL_T rx, ry;
	VAL_INT_T ix, iy;
	switch (bif_match_bin(rt, x_loc, y_loc, &ix, &iy, &rx, &ry)) {
	case BBM_BOTH_INT:
		rt_val_push_bool(rt->stack, ix < iy);
		break;
	case BBM_BOTH_REAL:
	case BBM_HETERO:
		rt_val_push_bool(rt->stack, rx < ry);
		break;
	case BBM_MISMATCH:
		bif_compare_error_arg_mismatch();
		return;
	}
}
