/* Copyright (C) 2014 Krzysztof Stachowiak */

#include "bif.h"
#include "bif_detail.h"
#include "value.h"
#include "stack.h"
#include "error.h"

static void bif_init_unary_array_ast(struct AstNode *node)
{
	node->type = AST_BIF;
	node->loc = bif_location;
	node->data.bif.func.formal_args = bif_arg_names;
	node->data.bif.func.arg_locs = bif_arg_locations;
	node->data.bif.func.arg_count = 1;
	node->data.bif.type = AST_BIF_ARRAY_UNARY;
	node->data.bif.un_int_impl = NULL;
	node->data.bif.un_real_impl = NULL;
	node->data.bif.bin_int_impl = NULL;
	node->data.bif.bin_real_impl = NULL;
	node->data.bif.un_arr_impl = NULL;
}

static void bif_car_impl(struct Stack* stack, VAL_LOC_T location)
{
	struct Value value = stack_peek_value(stack, location);
	struct ValueHeader head_header;
	VAL_LOC_T head_loc;

	if ((enum ValueType)value.header.type != VAL_ARRAY) {
		err_set(ERR_EVAL, "Array BIF called with non-array argument.");
		return;
	}

	if (value.compound.size == 0) {
		err_set(ERR_EVAL, "BIF \"car\" called for an empty array.");
		return;
	}

	head_loc = location + VAL_HEAD_BYTES + VAL_SIZE_BYTES;
	head_header = stack_peek_header(stack, head_loc);

	stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&head_header.type);
	stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&head_header.size);
	stack_push(stack, head_header.size,
			   stack->buffer + head_loc + VAL_HEAD_BYTES);

	/* TODO: consider this monadic scheme for simple BIFs as well:
	 * BIF :: value -> stack(value)
	 */
}

static void bif_cdr_impl(struct Stack* stack, VAL_LOC_T location)
{
	struct Value value = stack_peek_value(stack, location);
	struct ValueHeader head_header;
	VAL_LOC_T head_loc, tail_loc;
	VAL_HEAD_TYPE_T new_type;
	VAL_HEAD_SIZE_T new_size;
	VAL_SIZE_T new_count;

	if ((enum ValueType)value.header.type != VAL_ARRAY) {
		err_set(ERR_EVAL, "Array BIF called with non-array argument.");
		return;
	}

	if (value.compound.size == 0) {
		err_set(ERR_EVAL, "BIF \"cdr\" called for an empty array.");
		return;
	}

	head_loc = location + VAL_HEAD_BYTES + VAL_SIZE_BYTES;
	head_header = stack_peek_header(stack, head_loc);

	new_type = value.header.type;
	new_size = value.header.size - head_header.size - VAL_HEAD_BYTES;
	new_count = value.compound.size - 1;

	tail_loc = head_loc + VAL_HEAD_BYTES + head_header.size;

	stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&new_type);
	stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&new_size);
	stack_push(stack, VAL_SIZE_BYTES, (char*)&new_count);
	stack_push(stack, new_size, stack->buffer + tail_loc);
}

struct AstNode bif_car;
struct AstNode bif_cdr;

void bif_init_array(void)
{
	bif_init_unary_array_ast(&bif_car);
	bif_car.data.bif.un_arr_impl = bif_car_impl;

	bif_init_unary_array_ast(&bif_cdr);
	bif_cdr.data.bif.un_arr_impl = bif_cdr_impl;
}
