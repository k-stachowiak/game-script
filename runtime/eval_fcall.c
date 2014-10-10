/* Copyright (C) 2014 Krzysztof Stachowiak */

#include <string.h>

#include "eval.h"
#include "eval_detail.h"

#include "error.h"
#include "bif.h"
#include "runtime.h"
#include "rt_val.h"

static void fcall_error_too_many_args(char *symbol)
{
	struct ErrMessage msg;
	err_msg_init_src(&msg, "EVAL FUNC CALL", eval_location_top());
	err_msg_append(&msg, "Passed too many arguments to \"%s\"", symbol);
	err_msg_set(&msg);
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
        VAL_LOC_T *loc)
{
	struct SymMapKvp *kvp;

	if (!(kvp = sym_map_find(sym_map, symbol))) {
		eval_error_not_found(symbol);
		return false;
	}

    *loc = kvp->stack_loc;
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
        struct Runtime *rt,
        struct SymMap *sym_map,
		struct AstNode *actual_args,
        struct AstNode *impl,
        VAL_SIZE_T cap_count, VAL_LOC_T cap_loc,
        VAL_SIZE_T appl_count, VAL_LOC_T appl_loc)
{
	VAL_LOC_T current_loc, size_loc, data_begin;
	VAL_SIZE_T i, arg_count;

	stack_push_func_init(rt->stack, &size_loc, &data_begin, impl);

	/* Captures. */
	stack_push_func_cap_init(rt->stack, cap_count);
    current_loc = cap_loc;
	for (i = 0; i < cap_count; ++i) {
        stack_push_func_cap_copy(rt->stack, current_loc);
        current_loc += rt_val_fun_next_cap_loc(rt, current_loc);
	}

	/* Applied arguments. */
	arg_count = appl_count + ast_list_len(actual_args);

    /* ...already applied, */
	stack_push_func_appl_init(rt->stack, arg_count);
    current_loc = appl_loc;
	for (i = 0; i < appl_count; ++i) {
		stack_push_copy(rt->stack, current_loc);
        current_loc += rt_val_fun_next_appl_loc(rt, current_loc);
	}

    /* ...currently applied. */
	for (; actual_args; actual_args = actual_args->next) {
		eval_impl(actual_args, rt, sym_map);
		if (err_state()) {
			break;
		}
	}

	/* Finalize. */
	stack_push_func_final(rt->stack, size_loc, data_begin);
}

/** Evaluates an expression and inserts into a provided symbol map. */
static bool efc_insert_expression(
        struct Runtime *rt,
        struct SymMap *caller_sym_map,
		struct SymMap *new_sym_map,
		struct AstNode *arg_node,
        struct SourceLocation *arg_loc,
		char *symbol)
{
	VAL_LOC_T location = eval_impl(arg_node, rt, caller_sym_map);
	if (err_state()) {
		return false;
	}

	sym_map_insert(new_sym_map, symbol, location, arg_loc);
	if (err_state()) {
		return false;
	}

	return true;
}

/** Evaluates a general function implementation i.e. not BIF. */
static void efc_evaluate_general(
        struct Runtime *rt,
        struct SymMap *sym_map,
		struct AstNode *actual_args,
        struct AstNode *impl,
        VAL_SIZE_T cap_count, VAL_LOC_T cap_loc,
        VAL_SIZE_T appl_count, VAL_LOC_T appl_loc)
{
	VAL_SIZE_T i;
	VAL_LOC_T temp_begin, temp_end;
	struct SymMap local_sym_map;
	char **formal_args = impl->data.func_def.func.formal_args;
    struct SourceLocation *arg_locs = impl->data.func_def.func.arg_locs;
    struct SourceLocation cont_loc = { SRC_LOC_FUNC_CONTAINED, -1, -1 };

	/* Create the local scope. */
	if (sym_map->global) {
		sym_map_init_local(&local_sym_map, sym_map->global);
	} else {
		sym_map_init_local(&local_sym_map, sym_map);
	}

	/* Insert captures into the scope. */
	for (i = 0; i < cap_count; ++i) {
        char *cap_symbol = rt_val_peek_fun_cap_symbol(rt, cap_loc);
        VAL_LOC_T cap_val_loc = rt_val_fun_cap_loc(rt, cap_loc);
		sym_map_insert(&local_sym_map, cap_symbol, cap_val_loc, &cont_loc);
        cap_loc = rt_val_fun_next_cap_loc(rt, cap_loc);
		if (err_state()) {
			goto cleanup;
		}
	}

	/* Insert already applied arguments. */
	for (i = 0; i < appl_count; ++i) {
		sym_map_insert(&local_sym_map, *(formal_args++), appl_loc, &cont_loc);
        appl_loc = rt_val_fun_next_appl_loc(rt, appl_loc);
		if (err_state()) {
			goto cleanup;
		}
	}

	/* Evaluate and insert new arguments. */
	temp_begin = rt->stack->top;
	for (; actual_args; actual_args = actual_args->next) {
		if (!efc_insert_expression(rt, sym_map, &local_sym_map,
                actual_args, arg_locs++, *(formal_args++))) {
			goto cleanup;
		}
	}
	temp_end = rt->stack->top;

	/* Evaluate the function expression. */
	eval_impl(impl->data.func_def.expr, rt, &local_sym_map);

	/* Collapse the temporaries. */
	stack_collapse(rt->stack, temp_begin, temp_end);

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
		struct Runtime *rt,
		struct SymMap *sym_map,
		struct AstNode *actual_args,
        VAL_SIZE_T appl_count, VAL_LOC_T appl_loc,
		struct AstBif *impl)
{
	VAL_LOC_T arg_locs[BIF_MAX_ARITY];
	VAL_SIZE_T arg_count = 0, i;
	VAL_LOC_T temp_begin, temp_end;

	/* Peal out already applied args. */
	for (i = 0; i < appl_count; ++i) {
		arg_locs[arg_count] = appl_loc;
        appl_loc = rt_val_next_loc(rt, appl_loc);
		++arg_count;
	}

	/* Evaluate the missing args. */
	temp_begin = rt->stack->top;
	for (; actual_args; actual_args = actual_args->next) {
		VAL_LOC_T temp_loc;
		if (arg_count >= BIF_MAX_ARITY) {
			LOG_ERROR("Argument count mismatch.\n");
			exit(1);
		}

		temp_loc = eval_impl(actual_args, rt, sym_map);
		arg_locs[arg_count] = temp_loc;
		++arg_count;

		if (err_state()) {
			return;
		}
	}
	temp_end = rt->stack->top;

	/* Assert arguments count. */
	if (!efc_assert_bif_arg_count(impl->type, arg_count)) {
		LOG_ERROR("Invalid argument count passed to BIF.");
		exit(1);
	}

	/* Evaluate the function implementation. */
	switch (impl->type) {
	case AST_BIF_UNARY:
		impl->u_impl(rt, arg_locs[0]);
		break;

	case AST_BIF_BINARY:
		impl->bi_impl(rt, arg_locs[0], arg_locs[1]);
		break;

	case AST_BIF_TERNARY:
		impl->ter_impl(rt, arg_locs[0], arg_locs[1], arg_locs[2]);
		break;
	}

	/* Collapse the temporaries. */
	stack_collapse(rt->stack, temp_begin, temp_end);
}

void eval_func_call(
		struct AstNode *node,
		struct Runtime *rt,
		struct SymMap *sym_map)
{
    VAL_LOC_T val_loc, impl_loc, cap_start, appl_start;
    struct AstNode *impl;
    VAL_SIZE_T appl_count, cap_count;
	int arity, applied;
	struct AstNode *actual_args = node->data.func_call.actual_args;
	char *symbol = node->data.func_call.symbol;

	if (!efc_lookup_value(symbol, sym_map, &val_loc)) {
        return;
    }

    rt_val_function_locs(rt, val_loc, &impl_loc, &cap_start, &appl_start);
    impl = (struct AstNode*)rt_peek_ptr(rt, impl_loc);
    cap_count = rt_peek_size(rt, cap_start);
    cap_start += VAL_SIZE_BYTES;
    appl_count = rt_peek_size(rt, appl_start);
    appl_start += VAL_SIZE_BYTES;

	arity = efc_compute_arity(impl);
	applied = appl_count + ast_list_len(actual_args);

	if (arity > applied) {
		efc_curry_on(rt, sym_map, actual_args, impl,
                cap_count, cap_start, appl_count, appl_start);

	} else if (arity == applied) {
		switch (impl->type) {
		case AST_FUNC_DEF:
			efc_evaluate_general(rt, sym_map, actual_args, impl,
                    cap_count, cap_start, appl_count, appl_start);
			break;

		case AST_BIF:
			efc_evaluate_bif(rt, sym_map, actual_args,
                    appl_count, appl_start, &impl->data.bif);
			break;

		default:
			LOG_ERROR("Non-function AST node pointed by function value.\n");
			exit(1);
		}

	} else {
		fcall_error_too_many_args(symbol);

	}
}

