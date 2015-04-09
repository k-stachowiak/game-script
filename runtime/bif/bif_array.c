/* Copyright (C) 2014,2015 Krzysztof Stachowiak */

#include "log.h"
#include "eval.h"
#include "collection.h"
#include "bif.h"
#include "bif_detail.h"
#include "stack.h"
#include "runtime.h"
#include "rt_val.h"
#include "error.h"

static void bif_arr_error_homo(char *func)
{
    struct ErrMessage msg;
    err_msg_init_src(&msg, "BIF EVAL ARRAY", eval_location_top());
    err_msg_append(&msg, "Function _%s_ must produce homogenous result.", func);
    err_msg_set(&msg);
}

static void bif_arr_error_arg(int arg, char *func, char *condition)
{
    struct ErrMessage msg;
    err_msg_init_src(&msg, "BIF EVAL ARRAY", eval_location_top());
    err_msg_append(&msg, "Argument %d of _%s_ %s", arg, func, condition);
    err_msg_set(&msg);
}

static void bif_arr_error_range(char *condition)
{
    struct ErrMessage msg;
    err_msg_init_src(&msg, "BIF EVAL ARRAY", eval_location_top());
    err_msg_append(&msg, "In _slice_ : %s", condition);
    err_msg_set(&msg);
}

void bif_length(struct Runtime* rt, VAL_LOC_T location)
{
    if (rt_val_peek_type(rt, location) != VAL_ARRAY) {
        bif_arr_error_arg(1, "length", "must be an array");
        return;
    }

    rt_val_push_int(rt->stack, rt_val_cpd_len(rt, location));
}

void bif_reverse(struct Runtime* rt, VAL_LOC_T location)
{
    int i;
    VAL_SIZE_T size;
    VAL_LOC_T size_loc, current_loc, end_loc;
    struct { VAL_LOC_T *data; int cap, size; } locs = { NULL, 0, 0 };

    if (rt_val_peek_type(rt, location) != VAL_ARRAY) {
        bif_arr_error_arg(1, "reverse", "must be an array");
        return;
    }

    size = rt_val_peek_size(rt, location);
    current_loc = rt_val_cpd_first_loc(location);
    end_loc = current_loc + size;

    while (current_loc != end_loc) {
        ARRAY_APPEND(locs, current_loc);
        current_loc = rt_val_next_loc(rt, current_loc);
    }

    rt_val_push_array_init(rt->stack, &size_loc);
    for (i = 0; i < locs.size; ++i) {
        stack_push_copy(rt->stack, locs.data[locs.size - i - 1]);
    }
    rt_val_push_cpd_final(rt->stack, size_loc, size);

    ARRAY_FREE(locs);
}

void bif_cat(struct Runtime* rt, VAL_LOC_T x_loc, VAL_LOC_T y_loc)
{
    VAL_LOC_T size_loc, loc, end, result_loc = rt->stack->top;
    VAL_SIZE_T x_size, y_size;

    /* Assert input. */
    if (rt_val_peek_type(rt, x_loc) != VAL_ARRAY) {
        bif_arr_error_arg(1, "cat", "must be an array");
        return;
    }

    if (rt_val_peek_type(rt, y_loc) != VAL_ARRAY) {
        bif_arr_error_arg(2, "cat", "must be an array");
        return;
    }

    x_size = rt_val_peek_size(rt, x_loc);
    y_size = rt_val_peek_size(rt, y_loc);

    /* Build new header. */
    rt_val_push_array_init(rt->stack, &size_loc);

    /* Append X. */
    loc = rt_val_cpd_first_loc(x_loc);
    end = loc + x_size;
    while (loc != end) {
        stack_push_copy(rt->stack, loc);
        loc = rt_val_next_loc(rt, loc);
    }

    /* Append Y. */
    loc = rt_val_cpd_first_loc(y_loc);
    end = loc + y_size;
    while (loc != end) {
        stack_push_copy(rt->stack, loc);
        loc = rt_val_next_loc(rt, loc);
    }

    /* Finalize. */
    rt_val_push_cpd_final(rt->stack, size_loc, x_size + y_size);

    /* Validate homogenity. */
    if (!rt_val_compound_homo(rt, result_loc)) {
        bif_arr_error_homo("cat");
    }
}

void bif_at(
        struct Runtime *rt,
        VAL_LOC_T x_loc,
        VAL_LOC_T y_loc)
{
    VAL_INT_T index;
    VAL_LOC_T loc;
    int len, i;

    if (rt_val_peek_type(rt, x_loc) != VAL_ARRAY) {
        bif_arr_error_arg(1, "at", "must be an array");
        return;
    }

    if (rt_val_peek_type(rt, y_loc) != VAL_INT) {
        bif_arr_error_arg(2, "at", "must be an integer");
        return;
    }

    index = rt_val_peek_int(rt, y_loc);
    len = rt_val_cpd_len(rt, x_loc);

    if (index < 0 || index >= len) {
        bif_arr_error_range("index out of bounds");
        return;
    }

    loc = rt_val_cpd_first_loc(x_loc);
    for (i = 0; i < index; ++i) {
        loc = rt_val_next_loc(rt, loc);
    }

    stack_push_copy(rt->stack, loc);
}

void bif_slice(
        struct Runtime *rt,
        VAL_LOC_T x_loc,
        VAL_LOC_T y_loc,
        VAL_LOC_T z_loc)
{
    VAL_INT_T first, last;
    VAL_LOC_T size_loc, loc, end;
    VAL_SIZE_T index = 0, size = 0;

    /* Assert input. */
    if (rt_val_peek_type(rt, x_loc) != VAL_ARRAY) {
        bif_arr_error_arg(1, "slice", "must be an array");
        return;
    }

    if (rt_val_peek_type(rt, y_loc) != VAL_INT) {
        bif_arr_error_arg(2, "slice", "must be an integer");
        return;
    }

    if (rt_val_peek_type(rt, z_loc) != VAL_INT) {
        bif_arr_error_arg(3, "slice", "must be an integer");
        return;
    }

    first = rt_val_peek_int(rt, y_loc);
    last = rt_val_peek_int(rt, z_loc);

    if (first < 0 || last < 0) {
        bif_arr_error_range("slice delimiters must be non-negative.");
        return;
    }

    /* Build new header. */
    rt_val_push_array_init(rt->stack, &size_loc);

    /* Iterate over array */
    loc = rt_val_cpd_first_loc(x_loc);
    end = loc + rt_val_peek_size(rt, x_loc);
    while (loc != end) {
        if (index >= first && index < last) {
            stack_push_copy(rt->stack, loc);
            size += rt_val_peek_size(rt, loc) + VAL_HEAD_BYTES;
        }
        loc = rt_val_next_loc(rt, loc);
        ++index;
    }

    /* Assert validity of input in an awkward place. */
    if (first > last) {
        bif_arr_error_range("slice end must be greater or equal slice begin.");
        return;
    }

    if (last > index) {
        bif_arr_error_range("slice end must be within array bounds.");
        return;
    }

    /* Finalize. */
    rt_val_push_cpd_final(rt->stack, size_loc, size);
}

