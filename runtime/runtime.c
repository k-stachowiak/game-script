/* Copyright (C) 2014,2015 Krzysztof Stachowiak */

#include <stdlib.h>

#include "memory.h"
#include "stack.h"
#include "eval.h"
#include "error.h"
#include "runtime.h"
#include "rt_val.h"
#include "bif.h"

static void rt_free_stored(struct Runtime *rt)
{
	if (rt->node_store) {
		ast_node_free(rt->node_store);
	}
	rt->node_store = NULL;
}

static void rt_init(struct Runtime *rt, long stack)
{
    struct SymMap *gsm;

	rt->stack = stack_make(stack);
	rt->node_store = NULL;
	rt->eval_callback_data = NULL;
	rt->eval_callback_begin = NULL;
	rt->eval_callback_end = NULL;

    gsm = &rt->global_sym_map;

	sym_map_init_global(gsm);

    sym_map_insert(gsm, "sqrt", eval_bif(rt, bif_sqrt, 1), &bif_location);
    sym_map_insert(gsm, "+", eval_bif(rt, bif_add, 2), &bif_location);
    sym_map_insert(gsm, "-", eval_bif(rt, bif_sub, 2), &bif_location);
    sym_map_insert(gsm, "*", eval_bif(rt, bif_mul, 2), &bif_location);
    sym_map_insert(gsm, "/", eval_bif(rt, bif_div, 2), &bif_location);
	sym_map_insert(gsm, "%", eval_bif(rt, bif_mod, 2), &bif_location);
	sym_map_insert(gsm, "=", eval_bif(rt, bif_eq, 2), &bif_location);
	sym_map_insert(gsm, "<", eval_bif(rt, bif_lt, 2), &bif_location);
	sym_map_insert(gsm, ">", eval_bif(rt, bif_gt, 2), &bif_location);
	sym_map_insert(gsm, "<=", eval_bif(rt, bif_leq, 2), &bif_location);
	sym_map_insert(gsm, ">=", eval_bif(rt, bif_geq, 2), &bif_location);
	sym_map_insert(gsm, "&&", eval_bif(rt, bif_and, 2), &bif_location);
	sym_map_insert(gsm, "||", eval_bif(rt, bif_or, 2), &bif_location);
	sym_map_insert(gsm, "^^", eval_bif(rt, bif_xor, 2), &bif_location);
	sym_map_insert(gsm, "~~", eval_bif(rt, bif_not, 1), &bif_location);
	sym_map_insert(gsm, "length", eval_bif(rt, bif_length, 1), &bif_location);
	sym_map_insert(gsm, "empty", eval_bif(rt, bif_empty, 1), &bif_location);
	sym_map_insert(gsm, "car", eval_bif(rt, bif_car, 1), &bif_location);
	sym_map_insert(gsm, "cdr", eval_bif(rt, bif_cdr, 1), &bif_location);
	sym_map_insert(gsm, "reverse", eval_bif(rt, bif_reverse, 1), &bif_location);
	sym_map_insert(gsm, "cons", eval_bif(rt, bif_cons, 2), &bif_location);
	sym_map_insert(gsm, "cat", eval_bif(rt, bif_cat, 2), &bif_location);
	sym_map_insert(gsm, "slice", eval_bif(rt, bif_slice, 3), &bif_location);
}

static void rt_deinit(struct Runtime *rt)
{
	rt_free_stored(rt);
	sym_map_deinit(&rt->global_sym_map);
	stack_free(rt->stack);
}

struct Runtime *rt_make(long stack)
{
    struct Runtime *result = mem_malloc(sizeof(*result));
    rt_init(result, stack);
    return result;
}

void rt_reset(struct Runtime *rt)
{
	long stack = rt->stack->size;
	rt_deinit(rt);
	rt_init(rt, stack);
}

void rt_free(struct Runtime *rt)
{
	rt_deinit(rt);
    mem_free(rt);
}

void rt_save(struct Runtime *rt)
{
	rt->saved_loc = rt->stack->top;
	rt->saved_store = rt->node_store;
}

void rt_restore(struct Runtime *rt)
{
	while (rt->node_store != rt->saved_store) {
		struct AstNode *temp = rt->node_store;
		ast_node_free_one(temp);
		rt->node_store = rt->node_store->next;
	}

	stack_collapse(rt->stack, rt->saved_loc, rt->stack->top);
}

void rt_set_eval_callback(
		struct Runtime *rt,
		void *data,
		EvalCallback begin,
		EvalCallback end)
{
	rt->eval_callback_data = data;
	rt->eval_callback_begin = begin;
	rt->eval_callback_end = end;
}

void rt_consume_one(
        struct Runtime *rt,
        struct AstNode *ast,
        VAL_LOC_T *loc,
        struct AstNode **next)
{
	if (loc) {
		*loc = rt->stack->top;
	}
	if (next) {
		*next = ast->next;
	}

	eval(ast, rt, &rt->global_sym_map);

	if (err_state()) {
		ast_node_free_one(ast);
		return;

	} else if (ast->type == AST_BIND) {
		ast->next = rt->node_store;
		rt->node_store = ast;

	} else {
		rt->stack->top = *loc;
		ast_node_free_one(ast);
	}
}

bool rt_consume_list(struct Runtime *rt, struct AstNode *ast_list)
{
	struct AstNode *next;

	rt_save(rt);

	while (ast_list) {
		rt_consume_one(rt, ast_list, NULL, &next);
		if (err_state()) {
			rt_restore(rt);
			/* NOTE: ast_list is used for the iteration so upon error,
			* no nodes that have already been consumed will be freed.
			* They have actually been freed upon success of the consume function.
			*/
			ast_node_free(ast_list);
			return false;
		}
		ast_list = next;
	}
	return true;
}

void rt_for_each_stack_val(struct Runtime *rt, void(*f)(void*, VAL_LOC_T))
{
	stack_for_each(rt->stack, rt, f);
}

void rt_for_each_sym(struct Runtime *rt, void(*f)(void*, char*, VAL_LOC_T))
{
	sym_map_for_each(&rt->global_sym_map, rt, f);
}

