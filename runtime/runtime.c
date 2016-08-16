/* Copyright (C) 2014-2016 Krzysztof Stachowiak */

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
    sym_map_insert(sm, "+", eval_bif(rt, bif_add, 2));
    sym_map_insert(sm, "-", eval_bif(rt, bif_sub, 2));
    sym_map_insert(sm, "*", eval_bif(rt, bif_mul, 2));
    sym_map_insert(sm, "/", eval_bif(rt, bif_div, 2));
    sym_map_insert(sm, "%", eval_bif(rt, bif_mod, 2));
    sym_map_insert(sm, "sqrt", eval_bif(rt, bif_sqrt, 1));
    sym_map_insert(sm, "real", eval_bif(rt, bif_real, 1));
    sym_map_insert(sm, "floor", eval_bif(rt, bif_floor, 1));
    sym_map_insert(sm, "ceil", eval_bif(rt, bif_ceil, 1));
    sym_map_insert(sm, "round", eval_bif(rt, bif_round, 1));
    sym_map_insert(sm, "eq", eval_bif(rt, bif_eq, 2));
    sym_map_insert(sm, "lt", eval_bif(rt, bif_lt, 2));
    sym_map_insert(sm, "xor", eval_bif(rt, bif_xor, 2));
    sym_map_insert(sm, "not", eval_bif(rt, bif_not, 1));
    sym_map_insert(sm, "push_front", eval_bif(rt, bif_push_front, 2));
    sym_map_insert(sm, "push_back", eval_bif(rt, bif_push_back, 2));
    sym_map_insert(sm, "cat", eval_bif(rt, bif_cat, 2));
    sym_map_insert(sm, "length", eval_bif(rt, bif_length, 1));
    sym_map_insert(sm, "at", eval_bif(rt, bif_at, 2));
    sym_map_insert(sm, "slice", eval_bif(rt, bif_slice, 3));
    sym_map_insert(sm, "print", eval_bif(rt, bif_print, 1));
    sym_map_insert(sm, "format", eval_bif(rt, bif_format, 2));
    sym_map_insert(sm, "to_string", eval_bif(rt, bif_to_string, 1));
    sym_map_insert(sm, "parse", eval_bif(rt, bif_parse, 1));
    sym_map_insert(sm, "parse_bool", eval_bif(rt, bif_parse_bool, 1));
    sym_map_insert(sm, "parse_char", eval_bif(rt, bif_parse_char, 1));
    sym_map_insert(sm, "parse_int", eval_bif(rt, bif_parse_int, 1));
    sym_map_insert(sm, "parse_real", eval_bif(rt, bif_parse_real, 1));
    sym_map_insert(sm, "rand_ui", eval_bif(rt, bif_rand_ui, 2));
    sym_map_insert(sm, "rand_ur", eval_bif(rt, bif_rand_ur, 2));
    sym_map_insert(sm, "rand_ber", eval_bif(rt, bif_rand_ber, 1));
    sym_map_insert(sm, "rand_exp", eval_bif(rt, bif_rand_exp, 1));
    sym_map_insert(sm, "rand_gauss", eval_bif(rt, bif_rand_gauss, 2));
    sym_map_insert(sm, "rand_distr", eval_bif(rt, bif_rand_distr, 1));
    sym_map_insert(sm, "is_bool", eval_bif(rt, bif_is_bool, 1));
    sym_map_insert(sm, "is_int", eval_bif(rt, bif_is_int, 1));
    sym_map_insert(sm, "is_real", eval_bif(rt, bif_is_real, 1));
    sym_map_insert(sm, "is_char", eval_bif(rt, bif_is_char, 1));
    sym_map_insert(sm, "is_array", eval_bif(rt, bif_is_array, 1));
    sym_map_insert(sm, "is_tuple", eval_bif(rt, bif_is_tuple, 1));
    sym_map_insert(sm, "is_function", eval_bif(rt, bif_is_function, 1));
    sym_map_insert(sm, "is_pointer", eval_bif(rt, bif_is_pointer, 1));
}

static void rt_init(struct Runtime *rt)
{
    struct SymMap *gsm;

    stack_init(&rt->stack);
    rt->node_store = NULL;

    gsm = &rt->global_sym_map;
    sym_map_init_global(gsm);

    rt->debug = false;
    dbg_init(&rt->debugger);

    rt_init_bif(rt, gsm);
}

static void rt_deinit(struct Runtime *rt)
{
    dbg_deinit(&rt->debugger);
    rt_free_stored(rt);
    sym_map_deinit(&rt->global_sym_map);
    stack_deinit(&rt->stack);
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
    rt->saved_loc = rt->stack.top;
    rt->saved_store = rt->node_store;
}

void rt_restore(struct Runtime *rt)
{
    while (rt->node_store != rt->saved_store) {
        struct AstNode *temp = rt->node_store;
        ast_node_free_one(temp);
        rt->node_store = rt->node_store->next;
    }

    stack_collapse(&rt->stack, rt->saved_loc, rt->stack.top);
}

void rt_register_clif_handler(
        struct Runtime *rt,
        char *symbol,
        int arity,
        ClifHandler handler)
{
    sym_map_insert(
        &rt->global_sym_map,
        symbol,
        eval_clif(rt, handler, arity));
}

bool rt_consume_one(
        struct Runtime *rt,
        struct AstNode *ast,
        struct AstLocMap *alm,
        VAL_LOC_T *loc,
        struct AstNode **next)
{
    VAL_LOC_T begin, result;

    if (next) {
        *next = ast->next;
    }

    begin = rt->stack.top;
    result = eval(ast, rt, &rt->global_sym_map, alm);

    if (loc) {
        *loc = result;
    }

    if (err_state()) {
        err_push_src("RUNTIME", alm_try_get(alm, ast), "Failed consuming AST node");
        ast_node_free_one(ast);
        return false;

    } else if (ast->type == AST_SPECIAL && ast->data.special.type == AST_SPEC_BIND) {
        ast->next = rt->node_store;
        rt->node_store = ast;

    } else {
        rt->stack.top = begin; /* Discard result value to save the stack. */
        ast_node_free(ast);
    }

    return true;
}

bool rt_consume_list(
        struct Runtime *rt,
        struct AstNode *ast_list,
    struct AstLocMap *alm,
        VAL_LOC_T *last_loc)
{
    struct AstNode *next;

    rt_save(rt);

    while (ast_list) {
        if (!rt_consume_one(rt, ast_list, alm, last_loc, &next)) {
            rt_restore(rt);
            /* NOTE: ast_list is used for the iteration so upon error,
            * no nodes that have already been consumed will be freed.
            * They have actually been freed upon success of the consume function.
            */
            if (next) {
                ast_node_free(next);
            }
            return false;
        }
        ast_list = next;
    }
    return true;
}

