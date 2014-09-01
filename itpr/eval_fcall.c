/* Copyright (C) 2014 Krzysztof Stachowiak */

#include <string.h>

#include "eval.h"
#include "eval_detail.h"

#include "error.h"
#include "value.h"
#include "bif.h"

static VAL_HEAD_SIZE_T zero = 0;
static VAL_HEAD_TYPE_T type_int = (VAL_HEAD_TYPE_T)VAL_INT;
static VAL_HEAD_SIZE_T size_int = VAL_INT_BYTES;
static VAL_HEAD_TYPE_T type_real = (VAL_HEAD_TYPE_T)VAL_REAL;
static VAL_HEAD_SIZE_T size_real = VAL_REAL_BYTES;
static VAL_HEAD_TYPE_T type_bool = (VAL_HEAD_TYPE_T)VAL_BOOL;
static VAL_HEAD_SIZE_T size_bool = VAL_BOOL_BYTES;

/**
 * Performs a lookup for the called function in the symbol map.
 * Then performs the lookup of the function value on the stack.
 * Returns true upon success or false if the symbol or the value
 * lookup failed.
 */
static bool efc_lookup_value(
		struct AstNode *call_node,
		struct SymMap *sym_map,
		struct Stack *stack,
		struct Value *result)
{
	struct SymMapKvp *kvp;

	if (!(kvp = sym_map_find(sym_map, call_node->data.func_call.symbol))) {
		err_set(ERR_EVAL, "Requested function doesn't exist.");
		return false;
	}

	*result = stack_peek_value(stack, kvp->location);
	if (result->header.type != (VAL_HEAD_TYPE_T)VAL_FUNCTION) {
		err_set(ERR_EVAL, "Requested call to a non-function value.");
		return false;
	}

	return true;
}

/** Determines the arity of the given function definition. */
static int efc_compute_arity(struct AstNode *def_node)
{
	switch (def_node->type) {
	case AST_BIF:
		switch (def_node->data.bif.type) {
		case AST_BIF_ARYTHM_UNARY:
			return 1;
		case AST_BIF_ARYTHM_BINARY:
		case AST_BIF_COMPARE:
			return 2;
		default:
			printf("Unhandled bif type.\n");
			exit(1);
		}
		break;

	case AST_FUNC_DEF:
		return def_node->data.func_def.func.arg_count;
		break;

	default:
		printf("Non-function AST node pointed by function value.\n");
		exit(1);
	}
}

/** Pushes a capture object on the stack. */
static void efc_push_capture(struct Stack *stack, struct Capture *cap)
{
	VAL_SIZE_T len = strlen(cap->symbol);
	struct ValueHeader header = stack_peek_header(stack, cap->location);
	stack_push(stack, VAL_SIZE_BYTES, (char*)&len);
	stack_push(stack, len, cap->symbol);
	stack_push(stack,
		header.size + VAL_HEAD_BYTES,
		stack->buffer + cap->location);
}

/** Pushes a value pointed by a stack location on the top of the stack. */
static void efc_push_value(struct Stack *stack, VAL_LOC_T location)
{
	struct ValueHeader header = stack_peek_header(stack, location);
	stack_push(stack, header.size + VAL_HEAD_BYTES, stack->buffer + location);
}

/** Pushes an expression on the top of the stack by evaluating it. */
static bool efc_push_expression(
		struct Stack *stack,
		struct SymMap *sym_map,
		struct AstNode *expression)
{
	eval_impl(expression, stack, sym_map);
	return !err_state();
}

/**
 * Creates a new function value based on and existing function value,
 * Its captures, already evaluated arguments and the newly applied ones.
 */
static void efc_curry_on(
		struct Stack *stack,
		struct SymMap *sym_map,
		struct AstNode *actual_args,
		struct Value *value)
{
	static VAL_HEAD_TYPE_T type = (VAL_HEAD_TYPE_T)VAL_FUNCTION;
	VAL_LOC_T size_loc, data_begin, data_size;
	VAL_SIZE_T i, arg_count;

	/* Header. */
	stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type);
	size_loc = stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&zero);

	/* Pointer to implementation. */
	data_begin = stack_push(stack, VAL_PTR_BYTES, (char*)&value->function.def);

	/* Captures. */
	stack_push(stack, VAL_SIZE_BYTES, (char*)&value->function.captures.size);
	for (i = 0; i < value->function.captures.size; ++i) {
		efc_push_capture(stack, value->function.captures.data + i);
	}

	/* Applied arguments. */
	arg_count = value->function.applied.size + ast_list_len(actual_args);
	stack_push(stack, VAL_SIZE_BYTES, (char*)&arg_count);

	for (i = 0; i < value->function.applied.size; ++i) {
		efc_push_value(stack, value->function.applied.data[i]);
	}

	for (; actual_args; actual_args = actual_args->next) {
		if (!efc_push_expression(stack, sym_map, actual_args)) {
			break;
		}
	}

	/* Hack value size to correct value. */
	data_size = stack->top - data_begin;
	memcpy(stack->buffer + size_loc, &data_size, VAL_SIZE_BYTES);
}

/** Evaluates an expression and inserts into a provided symbol map. */
static bool efc_insert_expression(
		struct Stack *stack,
		struct SymMap *sym_map,
		struct AstNode *arg_node,
		char *symbol)
{
	VAL_LOC_T location = eval_impl(arg_node, stack, sym_map);
	if (err_state()) {
		return false;
	}
	sym_map_insert(sym_map, symbol, location);
	return true;
}

/** Evaluates a function implementation in a given context. */
static void efc_evaluate_impl(
		struct Stack *stack,
		struct SymMap *sym_map,
		struct AstNode *impl)
{
	for (; impl; impl = impl->next) {
		eval_impl(impl, stack, sym_map);
		if (err_state()) {
			return;
		}
	}
}

/** Evaluates a general function implementation i.e. not BIF. */
static void efc_evaluate_general(
		struct Stack *stack,
		struct SymMap *sym_map,
		struct AstNode *actual_args,
		struct Value *value)
{
	VAL_SIZE_T i;
	VAL_LOC_T temp_begin, temp_end;
	struct SymMap local_sym_map;
	struct AstNode *impl = value->function.def;
	char **formal_args = impl->data.func_def.func.formal_args;

	/* Create the local scope. */
	sym_map_init(&local_sym_map, sym_map, stack);

	/* Insert captures into the scope. */
	for (i = 0; i < value->function.captures.size; ++i) {
		struct Capture *cap = value->function.captures.data + i;
		sym_map_insert(&local_sym_map, cap->symbol, cap->location);
	}

	/* Insert already applied arguments. */
	for (i = 0; i < value->function.applied.size; ++i) {
		VAL_LOC_T loc = value->function.applied.data[i];
		sym_map_insert(&local_sym_map, *(formal_args++), loc);
	}

	/* Evaluate and insert new arguments. */
	temp_begin = stack->top;
	for (; actual_args; actual_args = actual_args->next) {
		if (!efc_insert_expression(
			stack, &local_sym_map, actual_args, *(formal_args++))) {
			goto cleanup;
		}
	}
	temp_end = stack->top;

	/* Evaluate the function expression. */
	efc_evaluate_impl(stack, sym_map, impl);

	/* Collapse the temporaries. */
	stack_collapse(stack, temp_begin, temp_end);

cleanup:

	/* Free the local scope. */
	sym_map_deinit(&local_sym_map);
}

/** Evaluates unary arythmetic BIF. */
static void efc_evaluate_bif_arythm_unary(
		struct Stack *stack,
		struct Value args[],
		struct AstBif *impl)
{
	if ((enum ValueType)args[0].header.type == VAL_INT) {
		VAL_INT_T result = impl->un_int_impl(args[0].primitive.integer);
		stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type_int);
		stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&size_int);
		stack_push(stack, size_int, (char*)&result);

	} else if ((enum ValueType)args[0].header.type == VAL_REAL) {
		VAL_REAL_T result = impl->un_real_impl(args[0].primitive.real);
		stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type_real);
		stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&size_real);
		stack_push(stack, size_real, (char*)&result);

	} else {
		err_set(ERR_EVAL, "Non-arythmetic type passed to arithmetic BIF.");
	}
}

/** Evaluates binary arythmetic BIF. */
static void efc_evaluate_bif_arythm_binary(
		struct Stack *stack,
		struct Value args[],
		struct AstBif *impl)
{
	if ((enum ValueType)args[0].header.type == VAL_INT &&
		(enum ValueType)args[1].header.type == VAL_INT) {
		VAL_INT_T result = impl->bin_int_impl(
			args[0].primitive.integer,
			args[1].primitive.integer);
		stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type_int);
		stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&size_int);
		stack_push(stack, size_int, (char*)&result);

	} else if ((enum ValueType)args[0].header.type == VAL_REAL &&
		     (enum ValueType)args[1].header.type == VAL_REAL) {
		VAL_REAL_T result = impl->bin_real_impl(
			args[0].primitive.real,
			args[1].primitive.real);
		stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type_real);
		stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&size_real);
		stack_push(stack, size_real, (char*)&result);

	} else {
		err_set(ERR_EVAL, "Argument types mismatch in an arithmetic BIF.");
	}
}

/** Evaluates comparison BIF. */
static void efc_evaluate_bif_compare(
		struct Stack *stack,
		struct Value args[],
		struct AstBif *impl)
{
	if ((enum ValueType)args[0].header.type == VAL_INT &&
		(enum ValueType)args[1].header.type == VAL_INT) {
		VAL_BOOL_T result = impl->cmp_int_impl(
			args[0].primitive.integer,
			args[1].primitive.integer);
		stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type_bool);
		stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&size_bool);
		stack_push(stack, size_bool, (char*)&result);

	} else if ((enum ValueType)args[0].header.type == VAL_REAL &&
		       (enum ValueType)args[1].header.type == VAL_REAL) {
		VAL_BOOL_T result = impl->cmp_real_impl(
			args[0].primitive.real,
			args[1].primitive.real);
		stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type_bool);
		stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&size_bool);
		stack_push(stack, size_bool, (char*)&result);

	} else {
		err_set(ERR_EVAL, "Argument types mismatch in a comparison BIF.");
	}
}

/** Tests the argument count against a BIF type. */
static bool efc_assert_arg_count(enum AstBifType type, int arg_count)
{
	switch (type) {
	case AST_BIF_ARYTHM_UNARY:
		return arg_count == 1;

	case AST_BIF_ARYTHM_BINARY:
		return arg_count == 2;

	case AST_BIF_COMPARE:
		return arg_count == 2;

	}

	printf("Unhandled BIF type.\n");
	exit(1);
}

/** Evaluates a BIF. */
static void efc_evaluate_bif(
		struct Stack *stack,
		struct SymMap *sym_map,
		struct AstNode *actual_args,
		struct Value *value)
{
	struct AstBif *impl = &value->function.def->data.bif;
	struct Value args[BIF_MAX_ARITY];
	VAL_SIZE_T arg_count = 0, i;
	VAL_LOC_T temp_begin, temp_end;

	/* Peal out already applied args. */
	for (i = 0; i < value->function.applied.size; ++i) {
		args[arg_count++] = stack_peek_value(
			stack, value->function.applied.data[i]);
	}

	/* Evaluate the missing args. */
	temp_begin = stack->top;
	for (; actual_args; actual_args = actual_args->next) {
		if (arg_count >= BIF_MAX_ARITY) {
			printf("Argument count mismatch.\n");
			exit(1);
		}
		args[arg_count++] = stack_peek_value(
			stack, eval_impl(actual_args, stack, sym_map));
		if (err_state()) {
			return;
		}
	}
	temp_end = stack->top;

	/* Assert arguments count. */
	if (!efc_assert_arg_count(impl->type, arg_count)) {
		err_set(ERR_EVAL, "Invalid argument count passed to BIF.");
		return;
	}

	/* Evaluate the function implementation. */
	switch (impl->type) {
	case AST_BIF_ARYTHM_UNARY:
		efc_evaluate_bif_arythm_unary(stack, args, impl);
		break;
	case AST_BIF_ARYTHM_BINARY:
		efc_evaluate_bif_arythm_binary(stack, args, impl);
		break;
	case AST_BIF_COMPARE:
		efc_evaluate_bif_compare(stack, args, impl);
		break;
	}

	/* Collapse the temporaries. */
	stack_collapse(stack, temp_begin, temp_end);
}

void eval_func_call(
		struct AstNode *node,
		struct Stack *stack,
		struct SymMap *sym_map)
{
	struct Value value;
	int arity, applied;
	struct AstNode *actual_args = node->data.func_call.actual_args;

	if (!efc_lookup_value(node, sym_map, stack, &value)) {
        return;
    }

	arity = efc_compute_arity(value.function.def);
	applied = value.function.applied.size + ast_list_len(actual_args);

	if (arity > applied) {
		efc_curry_on(stack, sym_map, actual_args, &value);

	} else if (arity == applied) {
		switch (value.function.def->type) {
		case AST_FUNC_DEF:
			efc_evaluate_general(stack, sym_map, actual_args, &value);
			break;

		case AST_BIF:
			efc_evaluate_bif(stack, sym_map, actual_args, &value);
			break;

		default:
			printf("Non-function AST node pointed by function value.\n");
			exit(1);
		}

	} else {
		err_set(ERR_EVAL, "Too many arguments provided to a function call.");

	}
}
