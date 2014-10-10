/* Copyright (C) 2014 Krzysztof Stachowiak */

#include "bif.h"
#include "bif_detail.h"
#include "stack.h"
#include "runtime.h"
#include "rt_val.h"

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
		struct Runtime *rt,
		VAL_LOC_T x_loc,
		VAL_INT_T *i,
		VAL_REAL_T *r)
{
    enum ValueType type = rt_val_peek_type(rt, x_loc);

	if (type == VAL_INT) {
		*i = rt_val_peek_int(rt, x_loc);
		return VAL_INT;

	} else if (type == VAL_REAL) {
		*r = rt_val_peek_real(rt, x_loc);
		return VAL_REAL;

	} else {
		return type;
	}
}

enum BifBinaryMatch bif_match_bin(
		struct Runtime *rt,
		VAL_LOC_T x_loc, VAL_LOC_T y_loc,
		VAL_INT_T *ix, VAL_INT_T *iy,
		VAL_REAL_T *rx, VAL_REAL_T *ry)
{
    enum ValueType x_type = rt_val_peek_type(rt, x_loc);
    enum ValueType y_type = rt_val_peek_type(rt, y_loc);

	if (x_type == VAL_INT && y_type == VAL_INT) {
		*ix = rt_val_peek_int(rt, x_loc);
		*iy = rt_val_peek_int(rt, y_loc);
		return BBM_BOTH_INT;

	} else if (x_type == VAL_REAL && y_type == VAL_REAL) {
		*rx = rt_val_peek_real(rt, x_loc);
		*ry = rt_val_peek_real(rt, y_loc);
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

