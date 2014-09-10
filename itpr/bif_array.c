/* Copyright (C) 2014 Krzysztof Stachowiak */

#include "bif.h"
#include "bif_detail.h"
#include "value.h"
#include "stack.h"
#include "error.h"

void bif_length_impl(struct Stack* stack, VAL_LOC_T location)
{
	struct Value value = stack_peek_value(stack, location);
	VAL_LOC_T end = location + VAL_HEAD_BYTES + value.header.size;
	VAL_INT_T result = 0;

	if ((enum ValueType)value.header.type != VAL_ARRAY) {
		err_eval_bif_array_arg(NULL, 1, "length", "must be an array");
		return;
	}
	
	location += VAL_HEAD_BYTES;
	while (location != end) {
		struct Value val = stack_peek_value(stack, location);
		location += val.header.size + VAL_HEAD_BYTES;
		++result;
	}

	stack_push_int(stack, result);
}

void bif_empty_impl(struct Stack* stack, VAL_LOC_T location)
{
	struct Value value = stack_peek_value(stack, location);
	VAL_BOOL_T result;

	if ((enum ValueType)value.header.type != VAL_ARRAY) {
		err_eval_bif_array_arg(NULL, 1, "empty", "must be an array");
		return;
	}

	result = (VAL_BOOL_T)(value.compound.size == 0);
	stack_push_bool(stack, result);
}

static void bif_car_impl(struct Stack* stack, VAL_LOC_T location)
{
	struct Value value = stack_peek_value(stack, location);
	VAL_LOC_T head_loc;

	if ((enum ValueType)value.header.type != VAL_ARRAY) {
		err_eval_bif_array_arg(NULL, 1, "car", "must be an array");
		return;
	}

	if (value.compound.size == 0) {
		err_eval_bif_array_arg(NULL, 1, "car", "must not be empty");
		return;
	}

	head_loc = location + VAL_HEAD_BYTES;
	stack_push_copy(stack, head_loc);
}

static void bif_cdr_impl(struct Stack* stack, VAL_LOC_T location)
{
	struct Value value;
	struct ValueHeader head_header;
	VAL_LOC_T head_loc, tail_loc;
	VAL_HEAD_TYPE_T new_type;
	VAL_HEAD_SIZE_T new_size;

	value = stack_peek_value(stack, location);

	if ((enum ValueType)value.header.type != VAL_ARRAY) {
		err_eval_bif_array_arg(NULL, 1, "cdr", "must be an array");
		return;
	}

	if (value.compound.size == 0) {
		err_eval_bif_array_arg(NULL, 1, "cdr", "must not be empty");
		return;
	}

	/* TODO: Consider generalization of this complex stack pushing below. */

	head_loc = location + VAL_HEAD_BYTES;
	head_header = stack_peek_header(stack, head_loc);

	new_type = value.header.type;
	new_size = value.header.size - head_header.size - VAL_HEAD_BYTES;

	tail_loc = head_loc + VAL_HEAD_BYTES + head_header.size;

	stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&new_type);
	stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&new_size);
	stack_push(stack, new_size, stack->buffer + tail_loc);
}

static void bif_reverse_impl(struct Stack* stack, VAL_LOC_T location)
{
	struct Value value;
	VAL_LOC_T size_loc, loc, end;
	VAL_SIZE_T size;
	int i; /* NOTE: This is counting down - keep it UNSIGNED! */

	struct {
		VAL_LOC_T *data;
		int cap, size;
	} locs = { NULL, 0, 0 };

	value = stack_peek_value(stack, location);
	size = value.header.size;

	/* Assert input. */
	if ((enum ValueType)value.header.type != VAL_ARRAY) {
		err_eval_bif_array_arg(NULL, 1, "reverse", "must be an array");
		return;
	}

	/* Store original locations. */
	loc = location + VAL_HEAD_BYTES;
	end = loc + size;
	while (loc != end) {
		struct ValueHeader header = stack_peek_header(stack, loc);
		ARRAY_APPEND(locs, loc);
		loc += header.size + VAL_HEAD_BYTES;
	}

	/* Push in reverse order. */
	stack_push_array_init(stack, &size_loc);
	for (i = locs.size - 1; i >= 0; --i) {
		stack_push_copy(stack, locs.data[i]);
	}
	stack_push_cpd_final(stack, size_loc, size);

	ARRAY_FREE(locs);
}

static void bif_cons_impl(struct Stack* stack, VAL_LOC_T x_loc, VAL_LOC_T y_loc)
{
	struct Value vx, vy;
	VAL_LOC_T size_loc, loc, end;

	/* Assert input. */
	vx = stack_peek_value(stack, x_loc);
	vy = stack_peek_value(stack, y_loc);

	if ((enum ValueType)vy.header.type != VAL_ARRAY) {
		/* 
		 * TODO: More refine check needed here:
		 * assert T = U, where
		 *    T  = (typeof x)
		 *   [U] = (typeof y)
		 */
		err_eval_bif_array_arg(NULL, 2, "cons", "must be an array");
		return;
	}

	/* Build new header. */
	stack_push_array_init(stack, &size_loc);

	/* Append x. */
	stack_push_copy(stack, x_loc);

	/* Append Y. */
	loc = y_loc + VAL_HEAD_BYTES;
	end = loc + vy.header.size;
	while (loc != end) {
		struct ValueHeader header = stack_peek_header(stack, loc);
		stack_push_copy(stack, loc);
		loc += header.size + VAL_HEAD_BYTES;
	}

	/* Finalize. */
	stack_push_cpd_final(
		stack, size_loc,
		vx.header.size + vy.header.size + VAL_HEAD_BYTES);
}

static void bif_cat_impl(struct Stack* stack, VAL_LOC_T x_loc, VAL_LOC_T y_loc)
{
	struct Value vx, vy;
	VAL_LOC_T size_loc, loc, end;

	/* Assert input. */
	vx = stack_peek_value(stack, x_loc);
	vy = stack_peek_value(stack, y_loc);

	if ((enum ValueType)vx.header.type != VAL_ARRAY) {
		err_eval_bif_array_arg(NULL, 1, "cat", "must be an array");
		return;
	}

	if ((enum ValueType)vy.header.type != VAL_ARRAY) {
		err_eval_bif_array_arg(NULL, 2, "cat", "must be an array");
		return;
	}

	/* Build new header. */
	stack_push_array_init(stack, &size_loc);

	/* Append X. */
	loc = x_loc + VAL_HEAD_BYTES;
	end = loc + vx.header.size;
	while (loc != end) {
		struct ValueHeader header = stack_peek_header(stack, loc);
		stack_push_copy(stack, loc);
		loc += header.size + VAL_HEAD_BYTES;
	}

	/* Append Y. */
	loc = y_loc + VAL_HEAD_BYTES;
	end = loc + vy.header.size;
	while (loc != end) {
		struct ValueHeader header = stack_peek_header(stack, loc);
		stack_push_copy(stack, loc);
		loc += header.size + VAL_HEAD_BYTES;
	}

	/* Finalize. */
	stack_push_cpd_final(stack, size_loc, vx.header.size + vy.header.size);
}

static void bif_slice_impl(
		struct Stack *stack,
		VAL_LOC_T x_loc,
		VAL_LOC_T y_loc,
		VAL_LOC_T z_loc)
{
	struct Value varr, vfirst, vlast;
	VAL_INT_T first, last;
	VAL_LOC_T size_loc, loc, end;
	VAL_SIZE_T index = 0, size = 0;

	/* Assert input. */
	varr = stack_peek_value(stack, x_loc);
	vfirst = stack_peek_value(stack, y_loc);
	vlast = stack_peek_value(stack, z_loc);

	if ((enum ValueType)varr.header.type != VAL_ARRAY) {
		err_eval_bif_array_arg(NULL, 1, "slice", "must be an array");
		return;
	}

	if ((enum ValueType)vfirst.header.type != VAL_INT) {
		err_eval_bif_array_arg(NULL, 2, "slice", "must be an integer");
		return;
	}

	if ((enum ValueType)vlast.header.type != VAL_INT) {
		err_eval_bif_array_arg(NULL, 3, "slice", "must be an integer");
		return;
	}

	first = vfirst.primitive.integer;
	last = vlast.primitive.integer;

	if (first < 0 || last < 0) {
		err_eval_bif_array_common(NULL, "Range of slice must be non-negative");
		return;
	}

	/* Build new header. */
	stack_push_array_init(stack, &size_loc);

	/* Iterate over array */
	loc = x_loc + VAL_HEAD_BYTES;
	end = loc + varr.header.size;
	while (loc != end) {
		struct ValueHeader header = stack_peek_header(stack, loc);
		if (index >= first && index < last) {
			stack_push_copy(stack, loc);
			size += header.size + VAL_HEAD_BYTES;
		}
		loc += header.size + VAL_HEAD_BYTES;
		++index;
	}

	/* Assert validity of input in an awkward place. */
	if (first > last) {
		err_eval_bif_array_arg(NULL, 3, "slice",
			"must be greater or equal argument 2");
		return;
	}

	if (last > index) {
		err_eval_bif_array_arg(NULL, 3, "slice",
			"must be within the bounds of the array in argument 1");
		return;
	}

	/* Finalize. */
	stack_push_cpd_final(stack, size_loc, size);
}

struct AstNode bif_length;
struct AstNode bif_empty;
struct AstNode bif_car;
struct AstNode bif_cdr;
struct AstNode bif_reverse;
struct AstNode bif_cons;
struct AstNode bif_cat;
struct AstNode bif_slice;

void bif_init_array(void)
{
	bif_init_unary_ast(&bif_length);
	bif_length.data.bif.u_impl = bif_length_impl;

	bif_init_unary_ast(&bif_empty);
	bif_empty.data.bif.u_impl = bif_empty_impl;

	bif_init_unary_ast(&bif_car);
	bif_car.data.bif.u_impl = bif_car_impl;

	bif_init_unary_ast(&bif_cdr);
	bif_cdr.data.bif.u_impl = bif_cdr_impl;

	bif_init_unary_ast(&bif_reverse);
	bif_reverse.data.bif.u_impl = bif_reverse_impl;

	bif_init_binary_ast(&bif_cons);
	bif_cons.data.bif.bi_impl = bif_cons_impl;

	bif_init_binary_ast(&bif_cat);
	bif_cat.data.bif.bi_impl = bif_cat_impl;

	bif_init_ternary_ast(&bif_slice);
	bif_slice.data.bif.ter_impl = bif_slice_impl;
}
