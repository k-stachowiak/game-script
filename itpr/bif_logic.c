/* Copyright (C) 2014 Krzysztof Stachowiak */

#include "bif.h"
#include "bif_detail.h"
#include "value.h"
#include "stack.h"
#include "error.h"

static void bif_init_logic_unary_ast(struct AstNode *node)
{
	node->type = AST_BIF;
	node->loc = bif_location;
	node->data.bif.func.formal_args = bif_arg_names;
	node->data.bif.func.arg_locs = bif_arg_locations;
	node->data.bif.func.arg_count = 1;
	node->data.bif.type = AST_BIF_LOGIC_UNARY;
	bif_init_impl_ptrs(node);
}

static void bif_init_logic_binary_ast(struct AstNode *node)
{
	node->type = AST_BIF;
	node->loc = bif_location;
	node->data.bif.func.formal_args = bif_arg_names;
	node->data.bif.func.arg_locs = bif_arg_locations;
	node->data.bif.func.arg_count = 2;
	node->data.bif.type = AST_BIF_LOGIC_BINARY;
	bif_init_impl_ptrs(node);
}

static VAL_BOOL_T bif_log_and(VAL_BOOL_T x, VAL_BOOL_T y) { return x && y; }
static VAL_BOOL_T bif_log_or(VAL_BOOL_T x, VAL_BOOL_T y) { return x || y; }
static VAL_BOOL_T bif_log_not(VAL_BOOL_T x) { return !x; }

static void common_bin_impl(
		struct Stack *stack,
		VAL_LOC_T x_loc, VAL_LOC_T y_loc,
		VAL_BOOL_T(*impl)(VAL_BOOL_T, VAL_BOOL_T))
{
	struct Value vx = stack_peek_value(stack, x_loc);
	struct Value vy = stack_peek_value(stack, y_loc);

	if ((enum ValueType)vx.header.type != VAL_BOOL ||
		(enum ValueType)vy.header.type != VAL_BOOL) {
		err_set(ERR_EVAL, "Logical BIF performed on a non-boolean values.");
		return;
	}

	stack_push_bool(stack, impl(vx.primitive.boolean, vy.primitive.boolean));
}

static void common_un_impl(
		struct Stack *stack,
		VAL_LOC_T x_loc,
		VAL_BOOL_T(*impl)(VAL_BOOL_T))
{
	struct Value vx = stack_peek_value(stack, x_loc);

	if ((enum ValueType)vx.header.type != VAL_BOOL) {
		err_set(ERR_EVAL, "Logical BIF performed on a non-boolean value.");
		return;
	}

	stack_push_bool(stack, impl(vx.primitive.boolean));
}

static void bif_and_impl(struct Stack* stack, VAL_LOC_T x_loc, VAL_LOC_T y_loc)
{
	common_bin_impl(stack, x_loc, y_loc, bif_log_and);
}

static void bif_or_impl(struct Stack* stack, VAL_LOC_T x_loc, VAL_LOC_T y_loc)
{
	common_bin_impl(stack, x_loc, y_loc, bif_log_or);
}

static void bif_not_impl(struct Stack* stack, VAL_LOC_T x_loc)
{
	common_un_impl(stack, x_loc, bif_log_not);
}

struct AstNode bif_and;
struct AstNode bif_or;
struct AstNode bif_not;

void bif_init_logic(void)
{
	bif_init_logic_binary_ast(&bif_and);
	bif_and.data.bif.bin_log_impl = bif_and_impl;

	bif_init_logic_binary_ast(&bif_or);
	bif_or.data.bif.bin_log_impl = bif_or_impl;

	bif_init_logic_unary_ast(&bif_not);
	bif_not.data.bif.un_log_impl = bif_not_impl;
}