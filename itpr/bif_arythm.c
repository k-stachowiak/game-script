/* Copyright (C) 2014 Krzysztof Stachowiak */

#include <stdbool.h>
#include <math.h>

#include "bif.h"
#include "bif_detail.h"
#include "stack.h"
#include "value.h"
#include "error.h"

static void bif_init_arythmetic_unary_ast(struct AstNode *node)
{
    node->type = AST_BIF;
    node->loc = bif_location;
    node->data.bif.func.formal_args = bif_arg_names;
    node->data.bif.func.arg_locs = bif_arg_locations;
    node->data.bif.func.arg_count = 1;
	node->data.bif.type = AST_BIF_ARYTHM_UNARY;
	bif_init_impl_ptrs(node);
}

static void bif_init_arythmetic_binary_ast(struct AstNode *node)
{
    node->type = AST_BIF;
    node->loc = bif_location;
    node->data.bif.func.formal_args = bif_arg_names;
    node->data.bif.func.arg_locs = bif_arg_locations;
    node->data.bif.func.arg_count = 2;
    node->data.bif.type = AST_BIF_ARYTHM_BINARY;
	node->data.bif.cmp_impl = NULL;
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

static void common_bin_impl(
		struct Stack *stack,
		VAL_LOC_T x_loc, VAL_LOC_T y_loc,
		VAL_INT_T(*impl_int)(VAL_INT_T, VAL_INT_T),
		VAL_REAL_T(*impl_real)(VAL_REAL_T, VAL_REAL_T))
{
	VAL_HEAD_TYPE_T type;
	VAL_HEAD_SIZE_T size;
	VAL_INT_T int_result;
	VAL_REAL_T real_result;
	char *result;

	VAL_REAL_T rx, ry;
	VAL_INT_T ix, iy;
	switch (bif_match_bin(stack, x_loc, y_loc, &ix, &iy, &rx, &ry)) {
	case BBM_BOTH_INT:
		type = (VAL_HEAD_TYPE_T)VAL_INT;
		size = VAL_INT_BYTES;
		int_result = impl_int(ix, iy);
		result = (char*)&int_result;
		break;
	case BBM_BOTH_REAL:
		type = (VAL_HEAD_TYPE_T)VAL_REAL;
		size = VAL_REAL_BYTES;
		real_result = impl_real(rx, ry);
		result = (char*)&real_result;
		break;
	case BBM_MISMATCH:
		err_set(ERR_EVAL, "Argument types mismatch in a comparison BIF.");
		return;
	}

	stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type);
	stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&size);
	stack_push(stack, size, (char*)&result);
}

static void common_un_impl(
		struct Stack *stack,
		VAL_LOC_T x_loc,
		VAL_INT_T(*impl_int)(VAL_INT_T),
		VAL_REAL_T(*impl_real)(VAL_REAL_T))
{
	VAL_HEAD_TYPE_T type;
	VAL_HEAD_SIZE_T size;
	VAL_INT_T int_result;
	VAL_REAL_T real_result;
	char *result;

	VAL_REAL_T r;
	VAL_INT_T i;
	switch (bif_match_un(stack, x_loc, &i, &r)) {
	case VAL_INT:
		type = (VAL_HEAD_TYPE_T)VAL_INT;
		size = VAL_INT_BYTES;
		int_result = impl_int(i);
		result = (char*)&int_result;
		break;
	case VAL_REAL:
		type = (VAL_HEAD_TYPE_T)VAL_REAL;
		size = VAL_REAL_BYTES;
		real_result = impl_real(r);
		result = (char*)&real_result;
		break;
	default:
		err_set(ERR_EVAL, "Arythmetic BIF called with non-numeric argument.");
		return;
	}

	stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type);
	stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&size);
	stack_push(stack, size, result);
}

static void bif_sqrt_impl(struct Stack* stack, VAL_LOC_T x_loc)
{
	common_un_impl(stack, x_loc, bif_sqrt_int, bif_sqrt_real);
}

static void bif_add_impl(struct Stack* stack, VAL_LOC_T x_loc, VAL_LOC_T y_loc)
{
	common_bin_impl(stack, x_loc, y_loc, bif_add_int, bif_add_real);
}

static void bif_sub_impl(struct Stack* stack, VAL_LOC_T x_loc, VAL_LOC_T y_loc)
{
	common_bin_impl(stack, x_loc, y_loc, bif_sub_int, bif_sub_real);
}

static void bif_mul_impl(struct Stack* stack, VAL_LOC_T x_loc, VAL_LOC_T y_loc)
{
	common_bin_impl(stack, x_loc, y_loc, bif_mul_int, bif_mul_real);
}

static void bif_div_impl(struct Stack* stack, VAL_LOC_T x_loc, VAL_LOC_T y_loc)
{
	common_bin_impl(stack, x_loc, y_loc, bif_div_int, bif_div_real);
}

struct AstNode bif_sqrt;

struct AstNode bif_add;
struct AstNode bif_sub;
struct AstNode bif_mul;
struct AstNode bif_div;

void bif_init_arythmetic(void)
{
	bif_init_arythmetic_unary_ast(&bif_sqrt);
	bif_sqrt.data.bif.un_arythm_impl = bif_sqrt_impl;

	bif_init_arythmetic_binary_ast(&bif_add);
	bif_add.data.bif.bin_arythm_impl = bif_add_impl;

	bif_init_arythmetic_binary_ast(&bif_sub);
	bif_sub.data.bif.bin_arythm_impl = bif_sub_impl;

	bif_init_arythmetic_binary_ast(&bif_mul);
	bif_mul.data.bif.bin_arythm_impl = bif_mul_impl;

	bif_init_arythmetic_binary_ast(&bif_div);
	bif_div.data.bif.bin_arythm_impl = bif_div_impl;
}