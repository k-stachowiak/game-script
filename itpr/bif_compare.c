/* Copyright (C) 2014 Krzysztof Stachowiak */

#include "bif.h"
#include "bif_detail.h"

static void bif_init_compare_ast(struct AstNode *node)
{
	node->type = AST_BIF;
	node->loc = bif_location;
	node->data.bif.func.formal_args = bif_arg_names;
	node->data.bif.func.arg_locs = bif_arg_locations;
	node->data.bif.func.arg_count = 2;
	node->data.bif.type = AST_BIF_COMPARE;
	node->data.bif.un_int_impl = NULL;
	node->data.bif.un_real_impl = NULL;
	node->data.bif.bin_int_impl = NULL;
	node->data.bif.bin_real_impl = NULL;
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

struct AstNode bif_eq;
struct AstNode bif_lt;
struct AstNode bif_gt;
struct AstNode bif_leq;
struct AstNode bif_geq;

void bif_init_compare(void)
{
	bif_init_compare_ast(&bif_eq);
	bif_eq.data.bif.cmp_int_impl = bif_eq_int;
	bif_eq.data.bif.cmp_real_impl = bif_eq_real;

	bif_init_compare_ast(&bif_lt);
	bif_lt.data.bif.cmp_int_impl = bif_lt_int;
	bif_lt.data.bif.cmp_real_impl = bif_lt_real;

	bif_init_compare_ast(&bif_gt);
	bif_gt.data.bif.cmp_int_impl = bif_gt_int;
	bif_gt.data.bif.cmp_real_impl = bif_gt_real;

	bif_init_compare_ast(&bif_leq);
	bif_leq.data.bif.cmp_int_impl = bif_leq_int;
	bif_leq.data.bif.cmp_real_impl = bif_leq_real;

	bif_init_compare_ast(&bif_geq);
	bif_geq.data.bif.cmp_int_impl = bif_geq_int;
	bif_geq.data.bif.cmp_real_impl = bif_geq_real;
}