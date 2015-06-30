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

static void bif_cpd_error_homo(char *func)
{
	err_push("BIF", "Function _%s_ must produce homogenous result.", func);
}

static void bif_cpd_error_arg(int arg, char *func, char *condition)
{
	err_push("BIF", "Argument %d of _%s_ %s", arg, func, condition);
}

static void bif_cpd_error_range(char *condition)
{
	err_push("BIF", "In _slice_ : %s", condition);
}

static void bif_cpd_error_mismatch(char *func)
{
	err_push("BIF", "Arguments of _%s_ must be of matching types", func);
}

void bif_push_front(struct Runtime* rt, VAL_LOC_T x_loc, VAL_LOC_T y_loc)
{
    int size, i;
    enum ValueType x_type = rt_val_peek_type(&rt->stack, x_loc);
    VAL_LOC_T size_loc, loc, result_loc = rt->stack.top;
    VAL_SIZE_T x_size = rt_val_peek_size(&rt->stack, x_loc);
    VAL_SIZE_T y_size = rt_val_peek_size(&rt->stack, y_loc);

    if (x_type != VAL_ARRAY && x_type != VAL_TUPLE) {
        bif_cpd_error_arg(1, "push-front", "must be compound");
        return;
    }

    if (x_type == VAL_ARRAY) {
        rt_val_push_array_init(&rt->stack, &size_loc);
    } else {
        rt_val_push_tuple_init(&rt->stack, &size_loc);
    }

    rt_val_push_copy(&rt->stack, y_loc);

    loc = rt_val_cpd_first_loc(x_loc);
    size = rt_val_cpd_len(rt, x_loc);
    for (i = 0; i < size; ++i) {
        rt_val_push_copy(&rt->stack, loc);
        loc = rt_val_next_loc(rt, loc);
    }

    rt_val_push_cpd_final(&rt->stack, size_loc, x_size + y_size + VAL_HEAD_BYTES);

    /* Validate homogenity. */
    if (x_type == VAL_ARRAY && !rt_val_compound_homo(rt, result_loc)) {
        bif_cpd_error_homo("push-front");
    }
}

void bif_push_back(struct Runtime *rt, VAL_LOC_T x_loc, VAL_LOC_T y_loc)
{
    int size, i;
    enum ValueType x_type = rt_val_peek_type(&rt->stack, x_loc);
    VAL_LOC_T size_loc, loc, result_loc = rt->stack.top;
    VAL_SIZE_T x_size = rt_val_peek_size(&rt->stack, x_loc);
    VAL_SIZE_T y_size = rt_val_peek_size(&rt->stack, y_loc);

    if (x_type != VAL_ARRAY && x_type != VAL_TUPLE) {
        bif_cpd_error_arg(1, "push-back", "must be compound");
        return;
    }

    if (x_type == VAL_ARRAY) {
        rt_val_push_array_init(&rt->stack, &size_loc);
    } else {
        rt_val_push_tuple_init(&rt->stack, &size_loc);
    }

    loc = rt_val_cpd_first_loc(x_loc);
    size = rt_val_cpd_len(rt, x_loc);
    for (i = 0; i < size; ++i) {
        rt_val_push_copy(&rt->stack, loc);
        loc = rt_val_next_loc(rt, loc);
    }

    rt_val_push_copy(&rt->stack, y_loc);

    rt_val_push_cpd_final(&rt->stack, size_loc, x_size + y_size + VAL_HEAD_BYTES);

    /* Validate homogenity. */
    if (x_type == VAL_ARRAY && !rt_val_compound_homo(rt, result_loc)) {
        bif_cpd_error_homo("push-back");
    }
}

void bif_cat(struct Runtime* rt, VAL_LOC_T x_loc, VAL_LOC_T y_loc)
{
    VAL_LOC_T size_loc, loc, end, result_loc = rt->stack.top;
    VAL_SIZE_T x_size, y_size;
    enum ValueType x_type = rt_val_peek_type(&rt->stack, x_loc);
    enum ValueType y_type = rt_val_peek_type(&rt->stack, y_loc);

    /* Assert input. */
    if (x_type != VAL_ARRAY && x_type != VAL_TUPLE) {
        bif_cpd_error_arg(1, "cat", "must be compound");
        return;
    }

    if (y_type != VAL_ARRAY && y_type != VAL_TUPLE) {
        bif_cpd_error_arg(2, "cat", "must be compound");
        return;
    }

    if (x_type != y_type) {
        bif_cpd_error_mismatch("cat");
        return;
    }

    x_size = rt_val_peek_size(&rt->stack, x_loc);
    y_size = rt_val_peek_size(&rt->stack, y_loc);

    /* Build new header. */
    if (x_type == VAL_ARRAY) {
        rt_val_push_array_init(&rt->stack, &size_loc);
    } else {
        rt_val_push_tuple_init(&rt->stack, &size_loc);
    }

    /* Append X. */
    loc = rt_val_cpd_first_loc(x_loc);
    end = loc + x_size;
    while (loc != end) {
        rt_val_push_copy(&rt->stack, loc);
        loc = rt_val_next_loc(rt, loc);
    }

    /* Append Y. */
    loc = rt_val_cpd_first_loc(y_loc);
    end = loc + y_size;
    while (loc != end) {
        rt_val_push_copy(&rt->stack, loc);
        loc = rt_val_next_loc(rt, loc);
    }

    /* Finalize. */
    rt_val_push_cpd_final(&rt->stack, size_loc, x_size + y_size);

    /* Validate homogenity. */
    if (x_type == VAL_ARRAY && !rt_val_compound_homo(rt, result_loc)) {
        bif_cpd_error_homo("cat");
    }
}

void bif_length(struct Runtime* rt, VAL_LOC_T location)
{
    enum ValueType type = rt_val_peek_type(&rt->stack, location);
    if (type != VAL_ARRAY && type != VAL_TUPLE) {
        bif_cpd_error_arg(1, "length", "must be compound");
        return;
    }

    rt_val_push_int(&rt->stack, rt_val_cpd_len(rt, location));
}

void bif_at(
        struct Runtime *rt,
        VAL_LOC_T x_loc,
        VAL_LOC_T y_loc)
{
    VAL_INT_T index;
    VAL_LOC_T loc;
    int len, i;
    enum ValueType x_type = rt_val_peek_type(&rt->stack, x_loc);

    if (x_type != VAL_ARRAY && x_type != VAL_TUPLE) {
        bif_cpd_error_arg(1, "at", "must be compound");
        return;
    }

    if (rt_val_peek_type(&rt->stack, y_loc) != VAL_INT) {
        bif_cpd_error_arg(2, "at", "must be an integer");
        return;
    }

    index = rt_val_peek_int(rt, y_loc);
    len = rt_val_cpd_len(rt, x_loc);

    if (index < 0 || index >= len) {
        bif_cpd_error_range("index out of bounds");
        return;
    }

    loc = rt_val_cpd_first_loc(x_loc);
    for (i = 0; i < index; ++i) {
        loc = rt_val_next_loc(rt, loc);
    }

    rt_val_push_copy(&rt->stack, loc);
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
    enum ValueType x_type = rt_val_peek_type(&rt->stack, x_loc);

    /* Assert input. */
    if (x_type != VAL_ARRAY && x_type != VAL_TUPLE) {
        bif_cpd_error_arg(1, "slice", "must be compound");
        return;
    }

    if (rt_val_peek_type(&rt->stack, y_loc) != VAL_INT) {
        bif_cpd_error_arg(2, "slice", "must be an integer");
        return;
    }

    if (rt_val_peek_type(&rt->stack, z_loc) != VAL_INT) {
        bif_cpd_error_arg(3, "slice", "must be an integer");
        return;
    }

    first = rt_val_peek_int(rt, y_loc);
    last = rt_val_peek_int(rt, z_loc);

    if (first < 0 || last < 0) {
        bif_cpd_error_range("slice delimiters must be non-negative.");
        return;
    }

    /* Build new header. */
    if (x_type == VAL_ARRAY) {
        rt_val_push_array_init(&rt->stack, &size_loc);
    } else {
        rt_val_push_tuple_init(&rt->stack, &size_loc);
    }

    /* Iterate over array */
    loc = rt_val_cpd_first_loc(x_loc);
    end = loc + rt_val_peek_size(&rt->stack, x_loc);
    while (loc != end) {
        if (index >= first && index < last) {
            rt_val_push_copy(&rt->stack, loc);
            size += rt_val_peek_size(&rt->stack, loc) + VAL_HEAD_BYTES;
        }
        loc = rt_val_next_loc(rt, loc);
        ++index;
    }

    /* Assert validity of input in an awkward place. */
    if (first > last) {
        bif_cpd_error_range("slice end must be greater or equal slice begin.");
        return;
    }

    if (last > index) {
        bif_cpd_error_range("slice end must be within array bounds.");
        return;
    }

    /* Finalize. */
    rt_val_push_cpd_final(&rt->stack, size_loc, size);
}

