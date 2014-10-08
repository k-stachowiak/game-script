/* Copyright (C) 2014 Krzysztof Stachowiak */

#include "eval.h"
#include "bif.h"
#include "bif_detail.h"
#include "stack.h"
#include "runtime.h"
#include "error.h"

static void bif_logic_error_arg_mismatch(void)
{
	struct ErrMessage msg;
	err_msg_init_src(&msg, "EVAL BIF LOGIC", eval_location_top());
	err_msg_append(&msg, "Arguments of logic BIF must be of boolean type");
	err_msg_set(&msg);
}

static VAL_BOOL_T bif_log_and(VAL_BOOL_T x, VAL_BOOL_T y) { return (bool)x && (bool)y; }
static VAL_BOOL_T bif_log_or(VAL_BOOL_T x, VAL_BOOL_T y) { return (bool)x || (bool)y; }
static VAL_BOOL_T bif_log_not(VAL_BOOL_T x) { return !((bool)x); }

static void common_bin_impl(
		struct Runtime *rt,
		VAL_LOC_T x_loc, VAL_LOC_T y_loc,
		VAL_BOOL_T(*impl)(VAL_BOOL_T, VAL_BOOL_T))
{
    enum ValueType x_type = rt_val_type(rt, x_loc);
    enum ValueType y_type = rt_val_type(rt, y_loc);

	if (x_type != VAL_BOOL || y_type != VAL_BOOL) {
		bif_logic_error_arg_mismatch();
		return;
	}

	stack_push_bool(rt->stack, impl(
            rt_peek_val_bool(rt, x_loc),
            rt_peek_val_bool(rt, y_loc)));
}

static void common_un_impl(
		struct Runtime *rt,
		VAL_LOC_T x_loc,
		VAL_BOOL_T(*impl)(VAL_BOOL_T))
{
    enum ValueType x_type = rt_val_type(rt, x_loc);

	if (x_type != VAL_BOOL) {
		bif_logic_error_arg_mismatch();
		return;
	}

	stack_push_bool(rt->stack, impl(rt_peek_val_bool(rt, x_loc)));
}

static void bif_and_impl(struct Runtime *rt, VAL_LOC_T x_loc, VAL_LOC_T y_loc)
{
	common_bin_impl(rt, x_loc, y_loc, bif_log_and);
}

static void bif_or_impl(struct Runtime *rt, VAL_LOC_T x_loc, VAL_LOC_T y_loc)
{
	common_bin_impl(rt, x_loc, y_loc, bif_log_or);
}

static void bif_not_impl(struct Runtime *rt, VAL_LOC_T x_loc)
{
	common_un_impl(rt, x_loc, bif_log_not);
}

struct AstNode bif_and;
struct AstNode bif_or;
struct AstNode bif_not;

void bif_init_logic(void)
{
	bif_init_binary_ast(&bif_and);
	bif_and.data.bif.bi_impl = bif_and_impl;

	bif_init_binary_ast(&bif_or);
	bif_or.data.bif.bi_impl = bif_or_impl;

	bif_init_unary_ast(&bif_not);
	bif_not.data.bif.u_impl = bif_not_impl;
}

