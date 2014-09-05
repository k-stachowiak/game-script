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
	bif_init_impl_ptrs(node);
}

void bif_size_impl(struct Stack* stack, VAL_LOC_T location)
{
	struct Value value = stack_peek_value(stack, location);
	VAL_LOC_T end = location + VAL_HEAD_BYTES + value.header.size;
	VAL_HEAD_TYPE_T type = (VAL_HEAD_TYPE_T)VAL_INT;
	VAL_HEAD_SIZE_T size = VAL_INT_BYTES;
	VAL_INT_T result = 0;

	if ((enum ValueType)value.header.type != VAL_ARRAY) {
		err_set(ERR_EVAL, "Array BIF called with non-array argument.");
		return;
	}
	
	location += VAL_HEAD_BYTES;
	while (location != end) {
		struct Value val = stack_peek_value(stack, location);
		location += val.header.size + VAL_HEAD_BYTES;
		++result;
	}

	stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type);
	stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&size);
	stack_push(stack, size, (char*)&result);
}

void bif_empty_impl(struct Stack* stack, VAL_LOC_T location)
{
	struct Value value = stack_peek_value(stack, location);
	VAL_HEAD_TYPE_T type = (VAL_HEAD_TYPE_T)VAL_BOOL;
	VAL_HEAD_SIZE_T size = VAL_BOOL_BYTES;
	VAL_BOOL_T result;

	if ((enum ValueType)value.header.type != VAL_ARRAY) {
		err_set(ERR_EVAL, "Array BIF called with non-array argument.");
		return;
	}

	result = (VAL_BOOL_T)(value.compound.size == 0);

	stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type);
	stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&size);
	stack_push(stack, size, (char*)&result);
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

	head_loc = location + VAL_HEAD_BYTES;
	head_header = stack_peek_header(stack, head_loc);

	stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&head_header.type);
	stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&head_header.size);
	stack_push(stack, head_header.size,
			   stack->buffer + head_loc + VAL_HEAD_BYTES);
}

static void bif_cdr_impl(struct Stack* stack, VAL_LOC_T location)
{
	struct Value value = stack_peek_value(stack, location);
	struct ValueHeader head_header;
	VAL_LOC_T head_loc, tail_loc;
	VAL_HEAD_TYPE_T new_type;
	VAL_HEAD_SIZE_T new_size;

	if ((enum ValueType)value.header.type != VAL_ARRAY) {
		err_set(ERR_EVAL, "Array BIF called with non-array argument.");
		return;
	}

	if (value.compound.size == 0) {
		err_set(ERR_EVAL, "BIF \"cdr\" called for an empty array.");
		return;
	}

	head_loc = location + VAL_HEAD_BYTES;
	head_header = stack_peek_header(stack, head_loc);

	new_type = value.header.type;
	new_size = value.header.size - head_header.size - VAL_HEAD_BYTES;

	tail_loc = head_loc + VAL_HEAD_BYTES + head_header.size;

	stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&new_type);
	stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&new_size);
	stack_push(stack, new_size, stack->buffer + tail_loc);
}

struct AstNode bif_size;
struct AstNode bif_empty;
struct AstNode bif_car;
struct AstNode bif_cdr;

void bif_init_array(void)
{
	bif_init_unary_array_ast(&bif_size);
	bif_size.data.bif.un_arr_impl = bif_size_impl;

	bif_init_unary_array_ast(&bif_empty);
	bif_empty.data.bif.un_arr_impl = bif_empty_impl;

	bif_init_unary_array_ast(&bif_car);
	bif_car.data.bif.un_arr_impl = bif_car_impl;

	bif_init_unary_array_ast(&bif_cdr);
	bif_cdr.data.bif.un_arr_impl = bif_cdr_impl;
}
