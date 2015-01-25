/* Copyright (C) 2014,2015 Krzysztof Stachowiak */

#include <string.h>

#include "eval.h"
#include "eval_detail.h"

#include "log.h"
#include "error.h"
#include "bif.h"
#include "runtime.h"
#include "rt_val.h"

#if RT_DEBUG
static void efc_log_gen_call_sym(char *symbol)
{
	printf("\tcall %s\n", symbol);
}

static void efc_log_gen_call_arg(struct Runtime *rt, VAL_LOC_T loc)
{
	printf("\t\t");
	rt_val_print(rt, loc, false);
	printf("\n");
}

static void efc_log_result(struct Runtime *rt, VAL_LOC_T loc)
{
	printf("\t>>");
	rt_val_print(rt, loc, false);
	printf("\n");
}

static void efc_log_bif_call(
		struct Runtime *rt,
		char *symbol,
		VAL_LOC_T arg_locs[],
		int arg_count,
		VAL_LOC_T result_loc)
{
	int i;
	efc_log_gen_call_sym(symbol);
	for (i = 0; i < arg_count; ++i) {
		efc_log_gen_call_arg(rt, arg_locs[i]);
	}
	efc_log_result(rt, result_loc);
}
#endif

static void fcall_error_too_many_args(char *symbol, int arity, int applied)
{
	struct ErrMessage msg;
	err_msg_init_src(&msg, "EVAL FUNC CALL", eval_location_top());
	err_msg_append(&msg, "Passed too many arguments to \"%s\".", symbol);
	err_msg_append(&msg, "Expected %d, applied %d", arity, applied);
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

/**
 * Creates a new function value based on and existing function value,
 * Its captures, already evaluated arguments and the newly applied ones.
 */
static void efc_curry_on(
        struct Runtime *rt,
        struct SymMap *sym_map,
		struct AstNode *actual_args,
        VAL_INT_T arity,
        struct AstNode *ast_def,
        void *bif_impl,
        VAL_SIZE_T cap_count, VAL_LOC_T cap_loc,
        VAL_SIZE_T appl_count, VAL_LOC_T appl_loc)
{
	VAL_LOC_T current_loc, size_loc, data_begin;
	VAL_SIZE_T i, arg_count;

	rt_val_push_func_init(rt->stack, &size_loc, &data_begin, arity, ast_def, bif_impl);

	/* Captures. */
	rt_val_push_func_cap_init(rt->stack, cap_count);
    current_loc = cap_loc;
	for (i = 0; i < cap_count; ++i) {
		rt_val_push_func_cap_copy(rt->stack, current_loc);
        current_loc += rt_val_fun_next_cap_loc(rt, current_loc);
	}

	/* Applied arguments. */
	arg_count = appl_count + ast_list_len(actual_args);

    /* ...already applied, */
	rt_val_push_func_appl_init(rt->stack, arg_count);
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
	rt_val_push_func_final(rt->stack, size_loc, data_begin);
}

/** Evaluates an expression and inserts into a provided symbol map. */
static bool efc_insert_expression(
        struct Runtime *rt,
        struct SymMap *caller_sym_map,
		struct SymMap *new_sym_map,
		struct AstNode *arg_node,
        struct SourceLocation *arg_loc,
		char *symbol,
		VAL_LOC_T *location)
{
	*location = eval_impl(arg_node, rt, caller_sym_map);
	if (err_state()) {
		return false;
	}

	sym_map_insert(new_sym_map, symbol, *location, arg_loc);
	if (err_state()) {
		return false;
	}

	return true;
}

/** Evaluates a general function implementation i.e. not BIF. */
static void efc_evaluate_ast(
        struct Runtime *rt,
        struct SymMap *sym_map,
		char *symbol, VAL_LOC_T func_loc, struct SourceLocation *func_src_loc,
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
	sym_map_insert(&local_sym_map, symbol, func_loc, func_src_loc);

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

	efc_log_gen_call_sym(symbol);

	/* Insert already applied arguments. */
	for (i = 0; i < appl_count; ++i) {
		efc_log_gen_call_arg(rt, appl_loc);
		sym_map_insert(&local_sym_map, *(formal_args++), appl_loc, &cont_loc);
        appl_loc = rt_val_fun_next_appl_loc(rt, appl_loc);
		if (err_state()) {
			goto cleanup;
		}
	}

	/* Evaluate and insert new arguments. */
	temp_begin = rt->stack->top;
	for (; actual_args; actual_args = actual_args->next) {
		VAL_LOC_T actual_loc;
		if (efc_insert_expression(rt, sym_map, &local_sym_map,
			actual_args, arg_locs++, *(formal_args++), &actual_loc)) {
			efc_log_gen_call_arg(rt, actual_loc);
		} else {
			goto cleanup;
		}
	}
	temp_end = rt->stack->top;

	/* Evaluate the function expression. */
	eval_impl(impl->data.func_def.expr, rt, &local_sym_map);

	efc_log_result(rt, temp_end);

	/* Collapse the temporaries. */
	stack_collapse(rt->stack, temp_begin, temp_end);

cleanup:

	/* Free the local scope. */
	sym_map_deinit(&local_sym_map);
}

/** Tests the argument count against a BIF type. */

/** Evaluates a BIF. */
static void efc_evaluate_bif(
		struct Runtime *rt,
		struct SymMap *sym_map,
		char *symbol,
		struct AstNode *actual_args,
        VAL_SIZE_T appl_count, VAL_LOC_T appl_loc,
        VAL_SIZE_T arity,
		void *impl)
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
	if (arity != arg_count) {
		LOG_ERROR("Invalid argument count passed to BIF.");
		exit(1);
	}

	/* Evaluate the function implementation. */
	switch (arity) {
	case 1:
        ((bif_unary_func)impl)(rt, arg_locs[0]);
		break;

	case 2:
		((bif_binary_func)impl)(rt, arg_locs[0], arg_locs[1]);
		break;

	case 3:
		((bif_ternary_func)impl)(rt, arg_locs[0], arg_locs[1], arg_locs[2]);
		break;
	}

	efc_log_bif_call(rt, symbol, arg_locs, arg_count, temp_end);

	/* Collapse the temporaries. */
	stack_collapse(rt->stack, temp_begin, temp_end);
}

void eval_func_call(
		struct AstNode *node,
		struct Runtime *rt,
		struct SymMap *sym_map)
{
    VAL_LOC_T
        val_loc, arity_loc,
        ast_def_loc, bif_impl_loc,
        cap_start, appl_start;

    struct AstNode *ast_def;
    void *bif_impl;
    VAL_SIZE_T appl_count, cap_count;
	VAL_SIZE_T arity, applied;
	struct AstNode *actual_args = node->data.func_call.actual_args;
	char *symbol = node->data.func_call.symbol;

	if (!efc_lookup_value(symbol, sym_map, &val_loc)) {
        return;
    }

    rt_val_function_locs(rt, val_loc,
            &arity_loc,
            &ast_def_loc, &bif_impl_loc,
            &cap_start, &appl_start);

    arity = stack_peek_size(rt->stack, arity_loc);
    ast_def = (struct AstNode*)stack_peek_ptr(rt->stack, ast_def_loc);
    bif_impl = (void*)stack_peek_ptr(rt->stack, bif_impl_loc);
    cap_count = stack_peek_size(rt->stack, cap_start);
    cap_start += VAL_SIZE_BYTES;
    appl_count = stack_peek_size(rt->stack, appl_start);
    appl_start += VAL_SIZE_BYTES;

	applied = appl_count + ast_list_len(actual_args);

	if (arity > applied) {
		efc_curry_on(rt, sym_map,
                actual_args,
                arity,
                ast_def, bif_impl,
                cap_count, cap_start,
                appl_count, appl_start);

	} else if (arity == applied) {
        if (ast_def && !bif_impl) {
			efc_evaluate_ast(
				rt, sym_map,
				symbol, val_loc, &ast_def->loc,
				actual_args,
				ast_def,
                cap_count, cap_start,
				appl_count, appl_start);

        } else if (!ast_def && bif_impl) {
			efc_evaluate_bif(rt, sym_map, symbol, actual_args,
                    appl_count, appl_start, arity, bif_impl);

        } else {
			LOG_ERROR("Malformed function value evaluated.\n");
			exit(1);
		}

	} else {
		fcall_error_too_many_args(symbol, arity, applied);

	}
}

