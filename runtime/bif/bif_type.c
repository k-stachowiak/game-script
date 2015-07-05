/* Copyright (C) 2015 Krzysztof Stachowiak */

#include "rt_val.h"
#include "runtime.h"

void bif_is_bool(struct Runtime *rt, VAL_LOC_T x_loc)
{
    rt_val_push_bool(
        &rt->stack,
        rt_val_peek_type(&rt->stack, x_loc) == VAL_BOOL);
}

void bif_is_int(struct Runtime *rt, VAL_LOC_T x_loc)
{
    rt_val_push_bool(
        &rt->stack,
        rt_val_peek_type(&rt->stack, x_loc) == VAL_INT);
}

void bif_is_real(struct Runtime *rt, VAL_LOC_T x_loc)
{
    rt_val_push_bool(
        &rt->stack,
        rt_val_peek_type(&rt->stack, x_loc) == VAL_REAL);
}

void bif_is_char(struct Runtime *rt, VAL_LOC_T x_loc)
{
    rt_val_push_bool(
        &rt->stack,
        rt_val_peek_type(&rt->stack, x_loc) == VAL_CHAR);
}

void bif_is_array(struct Runtime *rt, VAL_LOC_T x_loc)
{
    rt_val_push_bool(
        &rt->stack,
        rt_val_peek_type(&rt->stack, x_loc) == VAL_ARRAY);
}

void bif_is_tuple(struct Runtime *rt, VAL_LOC_T x_loc)
{
    rt_val_push_bool(
        &rt->stack,
        rt_val_peek_type(&rt->stack, x_loc) == VAL_TUPLE);
}

void bif_is_function(struct Runtime *rt, VAL_LOC_T x_loc)
{
    rt_val_push_bool(
        &rt->stack,
        rt_val_peek_type(&rt->stack, x_loc) == VAL_FUNCTION);
}

void bif_is_reference(struct Runtime *rt, VAL_LOC_T x_loc)
{
    rt_val_push_bool(
        &rt->stack,
        rt_val_peek_type(&rt->stack, x_loc) == VAL_REF);
}

