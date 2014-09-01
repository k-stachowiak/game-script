/* Copyright (C) 2014 Krzysztof Stachowiak */

#include <stdbool.h>
#include <math.h>

#include "bif.h"
#include "bif_detail.h"

static void bif_init_arythmetic_unary_ast(struct AstNode *node)
{
    node->type = AST_BIF;
    node->loc = bif_location;
    node->data.bif.func.formal_args = bif_arg_names;
    node->data.bif.func.arg_locs = bif_arg_locations;
    node->data.bif.func.arg_count = 1;
    node->data.bif.type = AST_BIF_ARYTHM_UNARY;
	node->data.bif.cmp_int_impl = NULL;
	node->data.bif.cmp_real_impl = NULL;
	node->data.bif.un_arr_impl = NULL;
}

static void bif_init_arythmetic_binary_ast(struct AstNode *node)
{
    node->type = AST_BIF;
    node->loc = bif_location;
    node->data.bif.func.formal_args = bif_arg_names;
    node->data.bif.func.arg_locs = bif_arg_locations;
    node->data.bif.func.arg_count = 2;
    node->data.bif.type = AST_BIF_ARYTHM_BINARY;
	node->data.bif.cmp_int_impl = NULL;
	node->data.bif.cmp_real_impl = NULL;
	node->data.bif.un_arr_impl = NULL;
}

static VAL_INT_T bif_sqrt_int(VAL_INT_T x) { return (VAL_INT_T)sqrt((double)x); }
static VAL_REAL_T bif_sqrt_real(VAL_REAL_T x) { return sqrt(x); }

static VAL_INT_T bif_add_int(VAL_INT_T x, VAL_INT_T y) { return x + y; }
static VAL_INT_T bif_sub_int(VAL_INT_T x, VAL_INT_T y) { return x - y; }
static VAL_INT_T bif_mul_int(VAL_INT_T x, VAL_INT_T y) { return x * y; }
static VAL_INT_T bif_div_int(VAL_INT_T x, VAL_INT_T y) { return x / y; }
static VAL_REAL_T bif_add_real(VAL_REAL_T x, VAL_REAL_T y) { return x + y; }
static VAL_REAL_T bif_sub_real(VAL_REAL_T x, VAL_REAL_T y) { return x - y; }
static VAL_REAL_T bif_mul_real(VAL_REAL_T x, VAL_REAL_T y) { return x * y; }
static VAL_REAL_T bif_div_real(VAL_REAL_T x, VAL_REAL_T y) { return x / y; }

struct AstNode bif_sqrt;

struct AstNode bif_add;
struct AstNode bif_sub;
struct AstNode bif_mul;
struct AstNode bif_div;

void bif_init_arythmetic(void)
{
	bif_init_arythmetic_unary_ast(&bif_sqrt);
	bif_sqrt.data.bif.un_int_impl = bif_sqrt_int;
	bif_sqrt.data.bif.un_real_impl = bif_sqrt_real;

	bif_init_arythmetic_binary_ast(&bif_add);
	bif_add.data.bif.bin_int_impl = bif_add_int;
	bif_add.data.bif.bin_real_impl = bif_add_real;

	bif_init_arythmetic_binary_ast(&bif_sub);
	bif_sub.data.bif.bin_int_impl = bif_sub_int;
	bif_sub.data.bif.bin_real_impl = bif_sub_real;

	bif_init_arythmetic_binary_ast(&bif_mul);
	bif_mul.data.bif.bin_int_impl = bif_mul_int;
	bif_mul.data.bif.bin_real_impl = bif_mul_real;

	bif_init_arythmetic_binary_ast(&bif_div);
	bif_div.data.bif.bin_int_impl = bif_div_int;
	bif_div.data.bif.bin_real_impl = bif_div_real;
}