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

    gsm = &rt->global_sym_map;

	sym_map_init_global(gsm);

    bif_assure_init();
    sym_map_insert(gsm, "sqrt", eval(&bif_sqrt, rt, gsm), &bif_location);
    sym_map_insert(gsm, "+", eval(&bif_add, rt, gsm), &bif_location);
    sym_map_insert(gsm, "-", eval(&bif_sub, rt, gsm), &bif_location);
    sym_map_insert(gsm, "*", eval(&bif_mul, rt, gsm), &bif_location);
    sym_map_insert(gsm, "/", eval(&bif_div, rt, gsm), &bif_location);
	sym_map_insert(gsm, "%", eval(&bif_mod, rt, gsm), &bif_location);
	sym_map_insert(gsm, "=", eval(&bif_eq, rt, gsm), &bif_location);
	sym_map_insert(gsm, "<", eval(&bif_lt, rt, gsm), &bif_location);
	sym_map_insert(gsm, ">", eval(&bif_gt, rt, gsm), &bif_location);
	sym_map_insert(gsm, "<=", eval(&bif_leq, rt, gsm), &bif_location);
	sym_map_insert(gsm, ">=", eval(&bif_geq, rt, gsm), &bif_location);
	sym_map_insert(gsm, "&&", eval(&bif_and, rt, gsm), &bif_location);
	sym_map_insert(gsm, "||", eval(&bif_or, rt, gsm), &bif_location);
	sym_map_insert(gsm, "~~", eval(&bif_not, rt, gsm), &bif_location);
	sym_map_insert(gsm, "length", eval(&bif_length, rt, gsm), &bif_location);
	sym_map_insert(gsm, "empty", eval(&bif_empty, rt, gsm), &bif_location);
	sym_map_insert(gsm, "car", eval(&bif_car, rt, gsm), &bif_location);
	sym_map_insert(gsm, "cdr", eval(&bif_cdr, rt, gsm), &bif_location);
	sym_map_insert(gsm, "reverse", eval(&bif_reverse, rt, gsm), &bif_location);
	sym_map_insert(gsm, "cons", eval(&bif_cons, rt, gsm), &bif_location);
	sym_map_insert(gsm, "cat", eval(&bif_cat, rt, gsm), &bif_location);
	sym_map_insert(gsm, "slice", eval(&bif_slice, rt, gsm), &bif_location);
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

