/* Copyright (C) 2014,2015 Krzysztof Stachowiak */

#include "log.h"
#include "eval.h"
#include "bif.h"
#include "bif_detail.h"
#include "stack.h"
#include "error.h"
#include "rt_val.h"

void bif_eq(struct Runtime *rt, VAL_LOC_T x_loc, VAL_LOC_T y_loc)
{
    rt_val_push_bool(&rt->stack, rt_val_eq_bin(rt, x_loc, y_loc));
}

void bif_lt(struct Runtime *rt, VAL_LOC_T x_loc, VAL_LOC_T y_loc)
{
    VAL_REAL_T rx, ry;
    VAL_INT_T ix, iy;
    switch (bif_match_bin(rt, x_loc, y_loc, &ix, &iy, &rx, &ry)) {
    case BBM_BOTH_INT:
        rt_val_push_bool(&rt->stack, ix < iy);
        break;
    case BBM_BOTH_REAL:
    case BBM_HETERO:
        rt_val_push_bool(&rt->stack, rx < ry);
        break;
    case BBM_MISMATCH:
		err_push("BIF", "Arguments of comparison BIF must be of equal numeric type");
        return;
    }
}
