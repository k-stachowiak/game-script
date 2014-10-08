/* Copyright (C) 2014 Krzysztof Stachowiak */

#include "eval.h"
#include "bif.h"
#include "bif_detail.h"
#include "stack.h"
#include "runtime.h"
#include "error.h"

static void bif_arr_error_arg(int arg, char *func, char *condition)
{
	struct ErrMessage msg;
	err_msg_init_src(&msg, "EVAL BIF ARRAY", eval_location_top());
	err_msg_append(&msg, "Argument %d of _%s_ %s", arg, func, condition);
	err_msg_set(&msg);
}

static void bif_arr_error_range(char *condition)
{
	struct ErrMessage msg;
	err_msg_init_src(&msg, "BIF EVAL", eval_location_top());
	err_msg_append(&msg, "In _slice_ : %s", condition);
	err_msg_set(&msg);
}

static void bif_length_impl(struct Runtime* rt, VAL_LOC_T location)
{
    VAL_LOC_T current, end;
    VAL_INT_T size = 0;

    if (rt_val_type(rt, location) != VAL_ARRAY) {
		bif_arr_error_arg(1, "length", "must be an array");
		return;
	}

    current = location + VAL_HEAD_BYTES;
    end = current + rt_val_size(rt, location);

    while (current != end) {
        current = rt_next_loc(rt, current);
        ++size;
    }

	stack_push_int(rt->stack, size);
}

static void bif_empty_impl(struct Runtime* rt, VAL_LOC_T location)
{
    if (rt_val_type(rt, location) != VAL_ARRAY) {
		bif_arr_error_arg(1, "empty", "must be an array");
	} else {
        stack_push_bool(rt->stack, rt_val_size(rt, location) == 0);
    }
}

static void bif_car_impl(struct Runtime* rt, VAL_LOC_T location)
{
    if (rt_val_type(rt, location) != VAL_ARRAY) {
		bif_arr_error_arg(1, "car", "must be an array");
		return;
	}

    if (rt_val_size(rt, location) == 0) {
		bif_arr_error_arg(1, "car", "must not be empty");
		return;
    }

	stack_push_copy(rt->stack, location + VAL_HEAD_BYTES);
}

static void bif_cdr_impl(struct Runtime* rt, VAL_LOC_T location)
{
    VAL_LOC_T head_loc, size_loc;
    VAL_LOC_T current_loc, end_loc;
    VAL_SIZE_T full_size, head_size, tail_size;

    if (rt_val_type(rt, location) != VAL_ARRAY) {
		bif_arr_error_arg(1, "cdr", "must be an array");
		return;
	}

    if (rt_val_size(rt, location) == 0) {
		bif_arr_error_arg(1, "cdr", "must not be empty");
		return;
    }

    head_loc = rt_peek_val_cpd_first(rt, location);

    full_size = rt_val_size(rt, location);
    head_size = rt_val_size(rt, head_loc);
    tail_size = full_size - head_size - VAL_HEAD_BYTES;

    current_loc = rt_next_loc(rt, head_loc);
    end_loc = head_loc + full_size;

    stack_push_array_init(rt->stack, &size_loc);
    while (current_loc != end_loc) {
        stack_push_copy(rt->stack, current_loc);
        current_loc = rt_next_loc(rt, current_loc);
    }
    stack_push_cpd_final(rt->stack, size_loc, tail_size);
}

static void bif_reverse_impl(struct Runtime* rt, VAL_LOC_T location)
{
    VAL_SIZE_T i, size;
    VAL_LOC_T size_loc, current_loc, end_loc;
	struct { VAL_LOC_T *data; int cap, size; } locs = { NULL, 0, 0 };

    if (rt_val_type(rt, location) != VAL_ARRAY) {
		bif_arr_error_arg(1, "reverse", "must be an array");
		return;
	}

    size = rt_val_size(rt, location);
    current_loc = rt_peek_val_cpd_first(rt, location);
    end_loc = current_loc + size;
    
    while (current_loc != end_loc) {
        ARRAY_APPEND(locs, current_loc);
        current_loc = rt_next_loc(rt, current_loc);
    }

    stack_push_array_init(rt->stack, &size_loc);
    for (i = 0; i < locs.size; ++i) {
        stack_push_copy(rt->stack, locs.data[locs.size - i - 1]);
    }
    stack_push_cpd_final(rt->stack, size_loc, size);

	ARRAY_FREE(locs);
}

static void bif_cons_impl(struct Runtime* rt, VAL_LOC_T x_loc, VAL_LOC_T y_loc)
{
	VAL_LOC_T size_loc, loc, end;
    VAL_SIZE_T x_size, y_size;

	if (rt_val_type(rt, y_loc) != VAL_ARRAY) {
		/* 
		 * TODO: More refine check needed here:
		 * assert T = U, where
		 *    T  = (typeof x)
		 *   [U] = (typeof y)
		 */
		bif_arr_error_arg(2, "cons", "must be an array");
		return;
	}

    x_size = rt_val_size(rt, x_loc);
    y_size = rt_val_size(rt, y_loc);

	/* Build new header. */
	stack_push_array_init(rt->stack, &size_loc);

	/* Append x. */
	stack_push_copy(rt->stack, x_loc);

	/* Append Y. */
	loc = rt_peek_val_cpd_first(rt, y_loc);
	end = loc + y_size;
	while (loc != end) {
		stack_push_copy(rt->stack, loc);
        loc = rt_next_loc(rt, loc);
	}

	/* Finalize. */
	stack_push_cpd_final(rt->stack, size_loc, x_size + y_size + VAL_HEAD_BYTES);
}

static void bif_cat_impl(struct Runtime* rt, VAL_LOC_T x_loc, VAL_LOC_T y_loc)
{
	VAL_LOC_T size_loc, loc, end;
    VAL_SIZE_T x_size, y_size;

	/* Assert input. */
	if (rt_val_type(rt, x_loc) != VAL_ARRAY) {
		bif_arr_error_arg(1, "cat", "must be an array");
		return;
	}

	if (rt_val_type(rt, y_loc) != VAL_ARRAY) {
		bif_arr_error_arg(2, "cat", "must be an array");
		return;
	}

    x_size = rt_val_size(rt, x_loc);
    y_size = rt_val_size(rt, y_loc);

	/* Build new header. */
	stack_push_array_init(rt->stack, &size_loc);

	/* Append X. */
	loc = rt_peek_val_cpd_first(rt, x_loc);
	end = loc + x_size;
	while (loc != end) {
		stack_push_copy(rt->stack, loc);
        loc = rt_next_loc(rt, loc);
	}

	/* Append Y. */
	loc = rt_peek_val_cpd_first(rt, y_loc);
	end = loc + y_size;
	while (loc != end) {
		stack_push_copy(rt->stack, loc);
        loc = rt_next_loc(rt, loc);
	}

	/* Finalize. */
	stack_push_cpd_final(rt->stack, size_loc, x_size + y_size);
}

static void bif_slice_impl(
		struct Runtime *rt,
		VAL_LOC_T x_loc,
		VAL_LOC_T y_loc,
		VAL_LOC_T z_loc)
{
	VAL_INT_T first, last;
	VAL_LOC_T size_loc, loc, end;
	VAL_SIZE_T index = 0, size = 0;

	/* Assert input. */
	if (rt_val_type(rt, x_loc) != VAL_ARRAY) {
		bif_arr_error_arg(1, "slice", "must be an array");
		return;
	}

	if (rt_val_type(rt, y_loc) != VAL_INT) {
		bif_arr_error_arg(2, "slice", "must be an integer");
		return;
	}

	if (rt_val_type(rt, z_loc) != VAL_INT) {
		bif_arr_error_arg(3, "slice", "must be an integer");
		return;
	}

	first = rt_peek_val_int(rt, y_loc);
	last = rt_peek_val_int(rt, z_loc);

	if (first < 0 || last < 0) {
		bif_arr_error_range("slice delimiters must be non-negative.");
		return;
	}

	/* Build new header. */
	stack_push_array_init(rt->stack, &size_loc);

	/* Iterate over array */
	loc = rt_peek_val_cpd_first(rt, x_loc);
	end = loc + rt_val_size(rt, x_loc);
	while (loc != end) {
		if (index >= first && index < last) {
			stack_push_copy(rt->stack, loc);
			size += rt_val_size(rt, loc) + VAL_HEAD_BYTES;
		}
        loc = rt_next_loc(rt, loc);
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
	stack_push_cpd_final(rt->stack, size_loc, size);
}

struct AstNode bif_length;
struct AstNode bif_empty;
struct AstNode bif_car;
struct AstNode bif_cdr;
struct AstNode bif_reverse;
struct AstNode bif_cons;
struct AstNode bif_cat;
struct AstNode bif_slice;

void bif_init_array(void)
{
	bif_init_unary_ast(&bif_length);
	bif_length.data.bif.u_impl = bif_length_impl;

	bif_init_unary_ast(&bif_empty);
	bif_empty.data.bif.u_impl = bif_empty_impl;

	bif_init_unary_ast(&bif_car);
	bif_car.data.bif.u_impl = bif_car_impl;

	bif_init_unary_ast(&bif_cdr);
	bif_cdr.data.bif.u_impl = bif_cdr_impl;

	bif_init_unary_ast(&bif_reverse);
	bif_reverse.data.bif.u_impl = bif_reverse_impl;

	bif_init_binary_ast(&bif_cons);
	bif_cons.data.bif.bi_impl = bif_cons_impl;

	bif_init_binary_ast(&bif_cat);
	bif_cat.data.bif.bi_impl = bif_cat_impl;

	bif_init_ternary_ast(&bif_slice);
	bif_slice.data.bif.ter_impl = bif_slice_impl;
}

