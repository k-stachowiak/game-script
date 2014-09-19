/* Copyright (C) 2014 Krzysztof Stachowiak */

#include "eval.h"
#include "bif.h"
#include "bif_detail.h"
#include "value.h"
#include "stack.h"
#include "error.h"

static void bif_compare_error_arg_mismatch(void)
{
	struct ErrMessage msg;
	err_msg_init(&msg, "EVAL BIF COMPARE", eval_location_top());
	err_msg_append(&msg, "Arguments of comparison BIF must be of equal numeric type");
	err_set_msg(&msg);
}

static VAL_BOOL_T bif_eq_int(VAL_INT_T x, VAL_INT_T y) { return x == y; }
static VAL_BOOL_T bif_lt_int(VAL_INT_T x, VAL_INT_T y) { return x < y; }
static VAL_BOOL_T bif_gt_int(VAL_INT_T x, VAL_INT_T y) { return x > y; }
static VAL_BOOL_T bif_leq_int(VAL_INT_T x, VAL_INT_T y) { return x <= y; }
static VAL_BOOL_T bif_geq_int(VAL_INT_T x, VAL_INT_T y) { return x >= y; }
static VAL_BOOL_T bif_eq_real(VAL_REAL_T x, VAL_REAL_T y) { return x == y; }
static VAL_BOOL_T bif_lt_real(VAL_REAL_T x, VAL_REAL_T y) { return x < y; }
static VAL_BOOL_T bif_gt_real(VAL_REAL_T x, VAL_REAL_T y) { return x > y; }
static VAL_BOOL_T bif_leq_real(VAL_REAL_T x, VAL_REAL_T y) { return x <= y; }
static VAL_BOOL_T bif_geq_real(VAL_REAL_T x, VAL_REAL_T y) { return x >= y; }

static void common_impl(
		struct Stack *stack,
		VAL_LOC_T x_loc, VAL_LOC_T y_loc,
		VAL_BOOL_T(*impl_int)(VAL_INT_T, VAL_INT_T),
		VAL_BOOL_T(*impl_real)(VAL_REAL_T, VAL_REAL_T))
{
	VAL_REAL_T rx, ry;
	VAL_INT_T ix, iy;
	switch (bif_match_bin(stack, x_loc, y_loc, &ix, &iy, &rx, &ry)) {
	case BBM_BOTH_INT:
		stack_push_bool(stack, impl_int(ix, iy));
		break;

	case BBM_BOTH_REAL:
		stack_push_bool(stack, impl_real(rx, ry));
		break;

	case BBM_MISMATCH:
		bif_compare_error_arg_mismatch();
		return;
	}
}

static void bif_eq_impl(struct Stack* stack, VAL_LOC_T x_loc, VAL_LOC_T y_loc)
{
	common_impl(stack, x_loc, y_loc, bif_eq_int, bif_eq_real);
}

static void bif_lt_impl(struct Stack* stack, VAL_LOC_T x_loc, VAL_LOC_T y_loc)
{
	common_impl(stack, x_loc, y_loc, bif_lt_int, bif_lt_real);
}

static void bif_gt_impl(struct Stack* stack, VAL_LOC_T x_loc, VAL_LOC_T y_loc)
{
	common_impl(stack, x_loc, y_loc, bif_gt_int, bif_gt_real);
}

static void bif_leq_impl(struct Stack* stack, VAL_LOC_T x_loc, VAL_LOC_T y_loc)
{
	common_impl(stack, x_loc, y_loc, bif_leq_int, bif_leq_real);
}

static void bif_geq_impl(struct Stack* stack, VAL_LOC_T x_loc, VAL_LOC_T y_loc)
{
	common_impl(stack, x_loc, y_loc, bif_geq_int, bif_geq_real);
}

struct AstNode bif_eq;
struct AstNode bif_lt;
struct AstNode bif_gt;
struct AstNode bif_leq;
struct AstNode bif_geq;

void bif_init_compare(void)
{
	bif_init_binary_ast(&bif_eq);
	bif_eq.data.bif.bi_impl = bif_eq_impl;

	bif_init_binary_ast(&bif_lt);
	bif_lt.data.bif.bi_impl = bif_lt_impl;

	bif_init_binary_ast(&bif_gt);
	bif_gt.data.bif.bi_impl = bif_gt_impl;

	bif_init_binary_ast(&bif_leq);
	bif_leq.data.bif.bi_impl = bif_leq_impl;

	bif_init_binary_ast(&bif_geq);
	bif_geq.data.bif.bi_impl = bif_geq_impl;
}

