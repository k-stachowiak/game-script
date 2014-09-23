/* Copyright (C) 2014 Krzysztof Stachowiak */

#include "bif.h"
#include "bif_detail.h"
#include "value.h"
#include "stack.h"

bool initialized = false;

struct SourceLocation bif_location = { SRC_LOC_BIF, -1, -1 };

struct SourceLocation bif_arg_locations[] = {
	{ SRC_LOC_BIF, -1, -1 },
	{ SRC_LOC_BIF, -1, -1 },
	{ SRC_LOC_BIF, -1, -1 }
};

char *bif_arg_names[] = { "x", "y", "z" };

static void bif_init_impl_ptrs(struct AstNode *node)
{
	node->data.bif.u_impl = NULL;
	node->data.bif.bi_impl = NULL;
	node->data.bif.ter_impl = NULL;
}

void bif_init_unary_ast(struct AstNode *node)
{
	node->type = AST_BIF;
	node->loc = bif_location;
	node->data.bif.func.formal_args = bif_arg_names;
	node->data.bif.func.arg_locs = bif_arg_locations;
	node->data.bif.func.arg_count = 1;
	node->data.bif.type = AST_BIF_UNARY;
	bif_init_impl_ptrs(node);
}

void bif_init_binary_ast(struct AstNode *node)
{
	node->type = AST_BIF;
	node->loc = bif_location;
	node->data.bif.func.formal_args = bif_arg_names;
	node->data.bif.func.arg_locs = bif_arg_locations;
	node->data.bif.func.arg_count = 2;
	node->data.bif.type = AST_BIF_BINARY;
	bif_init_impl_ptrs(node);
}

void bif_init_ternary_ast(struct AstNode *node)
{
	node->type = AST_BIF;
	node->loc = bif_location;
	node->data.bif.func.formal_args = bif_arg_names;
	node->data.bif.func.arg_locs = bif_arg_locations;
	node->data.bif.func.arg_count = 3;
	node->data.bif.type = AST_BIF_TERNARY;
	bif_init_impl_ptrs(node);
}

enum ValueType bif_match_un(
		struct Stack *stack,
		VAL_LOC_T x_loc,
		VAL_INT_T *i,
		VAL_REAL_T *r)
{
	struct Value x = stack_peek_value(stack, x_loc);

	if ((enum ValueType)x.header.type == VAL_INT) {
		*i = x.primitive.integer;
		return VAL_INT;

	} else if ((enum ValueType)x.header.type == VAL_REAL) {
		*r = x.primitive.real;
		return VAL_REAL;

	} else {
		return (enum ValueType)x.header.type;
	}
}

enum BifBinaryMatch bif_match_bin(
		struct Stack *stack,
		VAL_LOC_T x_loc, VAL_LOC_T y_loc,
		VAL_INT_T *ix, VAL_INT_T *iy,
		VAL_REAL_T *rx, VAL_REAL_T *ry)
{
	struct Value x = stack_peek_value(stack, x_loc);
	struct Value y = stack_peek_value(stack, y_loc);

	if ((enum ValueType)x.header.type == VAL_INT &&
		(enum ValueType)y.header.type == VAL_INT) {
		*ix = x.primitive.integer;
		*iy = y.primitive.integer;
		return BBM_BOTH_INT;

	} else if ((enum ValueType)x.header.type == VAL_REAL &&
		(enum ValueType)y.header.type == VAL_REAL) {
		*rx = x.primitive.real;
		*ry = y.primitive.real;
		return BBM_BOTH_REAL;

	} else {
		return BBM_MISMATCH;
	}
}

void bif_assure_init(void)
{
	if (initialized) {
		return;
	}

	initialized = true;
	bif_init_arythmetic();
	bif_init_compare();
	bif_init_logic();
	bif_init_array();
}

