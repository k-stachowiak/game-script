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

static void rt_init_bif(struct Runtime *rt, struct SymMap *sm)
{
	struct SourceLocation bif_location = src_loc_virtual();

    sym_map_insert(sm, "+", eval_bif(rt, bif_add, 2), bif_location);
    sym_map_insert(sm, "-", eval_bif(rt, bif_sub, 2), bif_location);
    sym_map_insert(sm, "*", eval_bif(rt, bif_mul, 2), bif_location);
    sym_map_insert(sm, "/", eval_bif(rt, bif_div, 2), bif_location);
    sym_map_insert(sm, "%", eval_bif(rt, bif_mod, 2), bif_location);
    sym_map_insert(sm, "sqrt", eval_bif(rt, bif_sqrt, 1), bif_location);
    sym_map_insert(sm, "real", eval_bif(rt, bif_real, 1), bif_location);
    sym_map_insert(sm, "floor", eval_bif(rt, bif_floor, 1), bif_location);
    sym_map_insert(sm, "ceil", eval_bif(rt, bif_ceil, 1), bif_location);
    sym_map_insert(sm, "round", eval_bif(rt, bif_round, 1), bif_location);
    sym_map_insert(sm, "eq", eval_bif(rt, bif_eq, 2), bif_location);
    sym_map_insert(sm, "lt", eval_bif(rt, bif_lt, 2), bif_location);
    sym_map_insert(sm, "xor", eval_bif(rt, bif_xor, 2), bif_location);
    sym_map_insert(sm, "not", eval_bif(rt, bif_not, 1), bif_location);
    sym_map_insert(sm, "push_front", eval_bif(rt, bif_push_front, 2), bif_location);
    sym_map_insert(sm, "push_back", eval_bif(rt, bif_push_back, 2), bif_location);
    sym_map_insert(sm, "cat", eval_bif(rt, bif_cat, 2), bif_location);
    sym_map_insert(sm, "length", eval_bif(rt, bif_length, 1), bif_location);
    sym_map_insert(sm, "at", eval_bif(rt, bif_at, 2), bif_location);
    sym_map_insert(sm, "slice", eval_bif(rt, bif_slice, 3), bif_location);
    sym_map_insert(sm, "print", eval_bif(rt, bif_print, 1), bif_location);
    sym_map_insert(sm, "format", eval_bif(rt, bif_format, 2), bif_location);
    sym_map_insert(sm, "to_string", eval_bif(rt, bif_to_string, 1), bif_location);
    sym_map_insert(sm, "parse", eval_bif(rt, bif_parse, 1), bif_location);
    sym_map_insert(sm, "parse_bool", eval_bif(rt, bif_parse_bool, 1), bif_location);
    sym_map_insert(sm, "parse_char", eval_bif(rt, bif_parse_char, 1), bif_location);
    sym_map_insert(sm, "parse_int", eval_bif(rt, bif_parse_int, 1), bif_location);
    sym_map_insert(sm, "parse_real", eval_bif(rt, bif_parse_real, 1), bif_location);
    sym_map_insert(sm, "rand_ui", eval_bif(rt, bif_rand_ui, 2), bif_location);
    sym_map_insert(sm, "rand_ur", eval_bif(rt, bif_rand_ur, 2), bif_location);
    sym_map_insert(sm, "rand_ber", eval_bif(rt, bif_rand_ber, 1), bif_location);
    sym_map_insert(sm, "rand_exp", eval_bif(rt, bif_rand_exp, 1), bif_location);
    sym_map_insert(sm, "rand_gauss", eval_bif(rt, bif_rand_gauss, 2), bif_location);
    sym_map_insert(sm, "rand_distr", eval_bif(rt, bif_rand_distr, 1), bif_location);
}

static void rt_init(struct Runtime *rt)
{
    struct SymMap *gsm;

    rt->stack = stack_make();
    rt->node_store = NULL;
    rt->eval_callback_data = NULL;
    rt->eval_callback_begin = NULL;
    rt->eval_callback_end = NULL;

    gsm = &rt->global_sym_map;
    sym_map_init_global(gsm);
    rt_init_bif(rt, gsm);
}

static void rt_deinit(struct Runtime *rt)
{
    rt_free_stored(rt);
    sym_map_deinit(&rt->global_sym_map);
    stack_free(rt->stack);
}

struct Runtime *rt_make(void)
{
    struct Runtime *result = mem_malloc(sizeof(*result));
    rt_init(result);
    return result;
}

void rt_reset(struct Runtime *rt)
{
    rt_deinit(rt);
    rt_init(rt);
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

void rt_reset_eval_callback(struct Runtime *rt)
{
    rt->eval_callback_data = NULL;
    rt->eval_callback_begin = NULL;
    rt->eval_callback_end = NULL;
}

void rt_set_eval_callback(
        struct Runtime *rt,
        void *data,
        EvalCallbackBegin begin,
        EvalCallbackEnd end)
{
    rt->eval_callback_data = data;
    rt->eval_callback_begin = begin;
    rt->eval_callback_end = end;
}

void rt_register_clif_handler(
		struct Runtime *rt,
		char *symbol,
		int arity,
		ClifIntraHandler handler)
{
    sym_map_insert(
		&rt->global_sym_map,
		symbol,
		eval_clif(rt, handler, arity),
		src_loc_virtual());
}

bool rt_consume_one(
        struct Runtime *rt,
        struct AstNode *ast,
        VAL_LOC_T *loc,
        struct AstNode **next)
{
    VAL_LOC_T begin, result;

    if (next) {
        *next = ast->next;
    }

    begin = rt->stack->top;
    result = eval(ast, rt, &rt->global_sym_map);

    if (loc) {
        *loc = result;
    }

    if (err_state()) {
		err_push_src("RUNTIME", ast->loc, "Failed consuming AST node");
        ast_node_free_one(ast);
        return false;

    } else if (ast->type == AST_BIND) {
        ast->next = rt->node_store;
        rt->node_store = ast;

    } else {
        rt->stack->top = begin; /* Discard result value to save the stack. */
        ast_node_free_one(ast);
    }

    return true;
}

bool rt_consume_list(
        struct Runtime *rt,
        struct AstNode *ast_list,
        VAL_LOC_T *last_loc)
{
    struct AstNode *next;

    rt_save(rt);

    while (ast_list) {
        if (!rt_consume_one(rt, ast_list, last_loc, &next)) {
            rt_restore(rt);
            /* NOTE: ast_list is used for the iteration so upon error,
            * no nodes that have already been consumed will be freed.
            * They have actually been freed upon success of the consume function.
            */
            if (ast_list->next) {
                ast_node_free(ast_list->next);
            }
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

