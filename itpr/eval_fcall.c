/* Copyright (C) 2014 Krzysztof Stachowiak */

#include <string.h>

#include "eval.h"
#include "eval_detail.h"

#include "error.h"
#include "value.h"
#include "bif.h"

static void fcall_error_nonfunc_value(char *symbol)
{
	struct ErrMessage msg;
	err_msg_init(&msg, "EVAL FUNC CALL", eval_location_top());
	err_msg_append(&msg, "Called non-function symbol \"%s\"", symbol);
	err_set_msg(&msg);
}

static void fcall_error_too_many_args(char *symbol)
{
	struct ErrMessage msg;
	err_msg_init(&msg, "EVAL FUNC CALL", eval_location_top());
	err_msg_append(&msg, "Passed too many arguments to \"%s\"", symbol);
	err_set_msg(&msg);
}

/**
 * Performs a lookup for the called function in the symbol map.
 * Then performs the lookup of the function value on the stack.
 * Returns true upon success or false if the symbol or the value
 * lookup failed.
 */
static bool efc_lookup_value(
		char *symbol,
		struct SymMap *sym_map,
		struct Stack *stack,
		struct Value *result)
{
	struct SymMapKvp *kvp;

	if (!(kvp = sym_map_find(sym_map, symbol))) {
		eval_error_not_found(symbol);
		return false;
	}

	*result = stack_peek_value(stack, kvp->location);
	if (result->header.type != (VAL_HEAD_TYPE_T)VAL_FUNCTION) {
		fcall_error_nonfunc_value(symbol);
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
		case AST_BIF_UNARY:
			return 1;

		case AST_BIF_BINARY:
			return 2;

		case AST_BIF_TERNARY:
			return 3;

		default:
			LOG_ERROR("Unhandled bif type.\n");
			exit(1);
		}
		break;

	case AST_FUNC_DEF:
		return def_node->data.func_def.func.arg_count;

	default:
		LOG_ERROR("Non-function AST node pointed by function value.\n");
		exit(1);
	}
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
	VAL_LOC_T size_loc, data_begin;
	VAL_SIZE_T i, arg_count;

	stack_push_func_init(
		stack,
		&size_loc,
		&data_begin,
		(void*)value->function.def);

	/* Captures. */
	stack_push_func_cap_init(stack, value->function.captures.size);
	for (i = 0; i < value->function.captures.size; ++i) {
		struct Capture *cap = value->function.captures.data + i;
		stack_push_func_cap(stack, cap->symbol, cap->location);
	}

	/* Applied arguments. */
	arg_count = value->function.applied.size + ast_list_len(actual_args);
	stack_push_func_appl_init(stack, arg_count);

	for (i = 0; i < value->function.applied.size; ++i) {
		stack_push_copy(stack, value->function.applied.data[i]);
	}

	for (; actual_args; actual_args = actual_args->next) {
		eval_impl(actual_args, stack, sym_map);
		if (err_state()) {
			break;
		}
	}

	/* Finalize. */
	stack_push_func_final(stack, size_loc, data_begin);
}

/** Evaluates an expression and inserts into a provided symbol map. */
static bool efc_insert_expression(
		struct Stack *stack,
		struct SymMap *sym_map,
		struct SymMap *new_sym_map,
		struct AstNode *arg_node,
		char *symbol)
{
	VAL_LOC_T location = eval_impl(arg_node, stack, sym_map);
	if (err_state()) {
		return false;
	}

	sym_map_insert(new_sym_map, symbol, location);
	if (err_state()) {
		return false;
	}

	return true;
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
	if (sym_map->global) {
		sym_map_init_local(&local_sym_map, sym_map->global);
	} else {
		sym_map_init_local(&local_sym_map, sym_map);
	}

	/* Insert captures into the scope. */
	for (i = 0; i < value->function.captures.size; ++i) {
		struct Capture *cap = value->function.captures.data + i;
		sym_map_insert(&local_sym_map, cap->symbol, cap->location);
		if (err_state()) {
			goto cleanup;
		}
	}

	/* Insert already applied arguments. */
	for (i = 0; i < value->function.applied.size; ++i) {
		VAL_LOC_T loc = value->function.applied.data[i];
		sym_map_insert(&local_sym_map, *(formal_args++), loc);
		if (err_state()) {
			goto cleanup;
		}
	}

	/* Evaluate and insert new arguments. */
	temp_begin = stack->top;
	for (; actual_args; actual_args = actual_args->next) {
		if (!efc_insert_expression(
			stack, sym_map, &local_sym_map, actual_args, *(formal_args++))) {
			goto cleanup;
		}
	}
	temp_end = stack->top;

	/* Evaluate the function expression. */
	eval_impl(impl->data.func_def.expr, stack, &local_sym_map);

	/* Collapse the temporaries. */
	stack_collapse(stack, temp_begin, temp_end);

cleanup:

	/* Free the local scope. */
	sym_map_deinit(&local_sym_map);
}

/** Tests the argument count against a BIF type. */
static bool efc_assert_bif_arg_count(enum AstBifType type, int arg_count)
{
	switch (type) {
	case AST_BIF_UNARY:
		return arg_count == 1;

	case AST_BIF_BINARY:
		return arg_count == 2;

	case AST_BIF_TERNARY:
		return arg_count == 3;
	}

	LOG_ERROR("Unhandled BIF type.\n");
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
	VAL_LOC_T arg_locs[BIF_MAX_ARITY];
	VAL_SIZE_T arg_count = 0, i;
	VAL_LOC_T temp_begin, temp_end;

	/* Peal out already applied args. */
	for (i = 0; i < value->function.applied.size; ++i) {
		arg_locs[arg_count] = value->function.applied.data[i];
		++arg_count;
	}

	/* Evaluate the missing args. */
	temp_begin = stack->top;
	for (; actual_args; actual_args = actual_args->next) {
		VAL_LOC_T temp_loc;
		if (arg_count >= BIF_MAX_ARITY) {
			LOG_ERROR("Argument count mismatch.\n");
			exit(1);
		}

		temp_loc = eval_impl(actual_args, stack, sym_map);
		arg_locs[arg_count] = temp_loc;
		++arg_count;

		if (err_state()) {
			return;
		}
	}
	temp_end = stack->top;

	/* Assert arguments count. */
	if (!efc_assert_bif_arg_count(impl->type, arg_count)) {
		LOG_ERROR("Invalid argument count passed to BIF.");
		exit(1);
	}

	/* Evaluate the function implementation. */
	switch (impl->type) {
	case AST_BIF_UNARY:
		impl->u_impl(stack, arg_locs[0]);
		break;

	case AST_BIF_BINARY:
		impl->bi_impl(stack, arg_locs[0], arg_locs[1]);
		break;

	case AST_BIF_TERNARY:
		impl->ter_impl(stack, arg_locs[0], arg_locs[1], arg_locs[2]);
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
	char *symbol = node->data.func_call.symbol;

	if (!efc_lookup_value(symbol, sym_map, stack, &value)) {
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
			LOG_ERROR("Non-function AST node pointed by function value.\n");
			exit(1);
		}

	} else {
		fcall_error_too_many_args(symbol);

	}
}

