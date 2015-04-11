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
    sym_map_insert(sm, "sqrt", eval_bif(rt, bif_sqrt, 1), &bif_location);
    sym_map_insert(sm, "+", eval_bif(rt, bif_add, 2), &bif_location);
    sym_map_insert(sm, "-", eval_bif(rt, bif_sub, 2), &bif_location);
    sym_map_insert(sm, "*", eval_bif(rt, bif_mul, 2), &bif_location);
    sym_map_insert(sm, "/", eval_bif(rt, bif_div, 2), &bif_location);
    sym_map_insert(sm, "%", eval_bif(rt, bif_mod, 2), &bif_location);
    sym_map_insert(sm, "floor", eval_bif(rt, bif_floor, 1), &bif_location);
    sym_map_insert(sm, "ceil", eval_bif(rt, bif_ceil, 1), &bif_location);
    sym_map_insert(sm, "round", eval_bif(rt, bif_round, 1), &bif_location);
    sym_map_insert(sm, "=", eval_bif(rt, bif_eq, 2), &bif_location);
    sym_map_insert(sm, "<", eval_bif(rt, bif_lt, 2), &bif_location);
    sym_map_insert(sm, "^", eval_bif(rt, bif_xor, 2), &bif_location);
    sym_map_insert(sm, "!", eval_bif(rt, bif_not, 1), &bif_location);
    sym_map_insert(sm, "length", eval_bif(rt, bif_length, 1), &bif_location);
    sym_map_insert(sm, "reverse", eval_bif(rt, bif_reverse, 1), &bif_location);
    sym_map_insert(sm, "push-front", eval_bif(rt, bif_push_front, 2), &bif_location);
    sym_map_insert(sm, "push-back", eval_bif(rt, bif_push_back, 2), &bif_location);
    sym_map_insert(sm, "cat", eval_bif(rt, bif_cat, 2), &bif_location);
    sym_map_insert(sm, "at", eval_bif(rt, bif_at, 2), &bif_location);
    sym_map_insert(sm, "slice", eval_bif(rt, bif_slice, 3), &bif_location);
    sym_map_insert(sm, "print", eval_bif(rt, bif_print, 1), &bif_location);
    sym_map_insert(sm, "format", eval_bif(rt, bif_format, 2), &bif_location);
    sym_map_insert(sm, "to-string", eval_bif(rt, bif_to_string, 1), &bif_location);
    sym_map_insert(sm, "parse", eval_bif(rt, bif_parse, 1), &bif_location);
    sym_map_insert(sm, "parse-bool", eval_bif(rt, bif_parse_bool, 1), &bif_location);
    sym_map_insert(sm, "parse-char", eval_bif(rt, bif_parse_char, 1), &bif_location);
    sym_map_insert(sm, "parse-int", eval_bif(rt, bif_parse_int, 1), &bif_location);
    sym_map_insert(sm, "parse-real", eval_bif(rt, bif_parse_real, 1), &bif_location);
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

    eval_init();
    rt_init_bif(rt, gsm);
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

void rt_consume_one(
        struct Runtime *rt,
        struct AstNode *ast,
        VAL_LOC_T *loc,
        struct AstNode **next)
{
    VAL_LOC_T result;

    if (next) {
        *next = ast->next;
    }

    result = eval(ast, rt, &rt->global_sym_map);

    if (loc) {
        *loc = result;
    }

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

