/* Copyright (C) 2014-2016 Krzysztof Stachowiak */

#include <stdbool.h>
#include <inttypes.h>

#include "error.h"
#include "eval.h"
#include "eval_detail.h"
#include "rt_val.h"

static void spec_error_arg_expected(
        char *func,
        int index,
        char *expected,
        struct SourceLocation *loc)
{
    err_push_src(
        "EVAL",
        loc,
        "Argument %d of special form _%s_ must be %s",
        index,
        func,
        expected);
}

static void eval_special_do(
        struct AstNode *node,
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct AstLocMap *alm)
{
    struct SymMap local_sym_map;
    struct AstSpecDo *doo = &node->data.special.data.doo;
    struct AstNode *expr = doo->exprs;

    sym_map_init_local(&local_sym_map, sym_map);

    VAL_LOC_T begin = rt->stack.top;
    VAL_LOC_T end = rt->stack.top;

    for (; expr; expr = expr->next) {
        VAL_LOC_T new_end = eval_dispatch(expr, rt, &local_sym_map, alm);
        if (err_state()) {
            err_push_src(
                "EVAL",
                alm_try_get(alm, expr),
                "Failed evaluating do expression");
            break;
        } else {
            end = new_end;
        }
    }

    stack_collapse(&rt->stack, begin, end);

    sym_map_deinit(&local_sym_map);
}

static void eval_special_match(
        struct AstNode *node,
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct AstLocMap *alm)
{
    struct AstSpecMatch *match = &node->data.special.data.match;
    struct AstNode *expr = match->expr;
    struct AstNode *key = match->keys;
    struct AstNode *value = match->values;
    VAL_LOC_T temp_begin, temp_end;

    temp_begin = rt->stack.top;
    VAL_LOC_T location = eval_dispatch(expr, rt, sym_map, alm);
    if (err_state()) {
        err_push_src(
            "EVAL",
            alm_try_get(alm, expr),
            "Failed evaluating match expression");
        return;
    }
    temp_end = rt->stack.top;

    while (key && value) {
        struct SymMap local_sym_map;
        sym_map_init_local(&local_sym_map, sym_map);

        eval_special_bind_pattern(key, location, rt, &local_sym_map, alm);

        if (err_state()) {
            err_reset(); /* We ignore all errors while in this loop. */
            key = key->next;
            value = value->next;
        } else {
            eval_dispatch(value, rt, &local_sym_map, alm);
            goto end;
        }

        sym_map_deinit(&local_sym_map);
    }


    /* Overzaleous sanity check. */
    if (key || value) {
        LOG_ERROR("Match expression AST inconsistency unacceptable here.");
        exit(1);
    }

    /* No match found. */
    err_push_src(
        "EVAL",
        alm_try_get(alm, node),
        "None of the cases were matched in match expression");

end:
    stack_collapse(&rt->stack, temp_begin, temp_end);
}

static void eval_special_if(
        struct AstNode* node,
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct AstLocMap *alm)
{
    VAL_LOC_T test_loc, temp_begin, temp_end;
    VAL_BOOL_T test_val;

    struct AstSpecIf *iff = &node->data.special.data.iff;

    temp_begin = rt->stack.top;
    test_loc = eval_dispatch(iff->test, rt, sym_map, alm);
    temp_end = rt->stack.top;

    if (err_state()) {
        err_push_src(
            "EVAL",
            alm_try_get(alm, iff->test),
            "Failed evaluating if test");
        return;
    }

    if (rt_val_peek_type(&rt->stack, test_loc) != VAL_BOOL) {
        spec_error_arg_expected("if", 1, "boolean", alm_try_get(alm, iff->test));
        stack_collapse(&rt->stack, temp_begin, temp_end);
        return;
    }

    test_val = rt_val_peek_bool(rt, test_loc);
    stack_collapse(&rt->stack, temp_begin, temp_end);

    if (test_val) {
        eval_dispatch(iff->true_expr, rt, sym_map, alm);
    } else {
        eval_dispatch(iff->false_expr, rt, sym_map, alm);
    }
}

static void eval_special_while(
        struct AstNode* node,
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct AstLocMap *alm)
{
    struct AstSpecWhile *whilee = &node->data.special.data.whilee;
    bool done = false;

    while (!done) {

        VAL_LOC_T test_loc, temp_begin, temp_end;
        VAL_BOOL_T test_val;

        temp_begin = rt->stack.top;
        test_loc = eval_dispatch(whilee->test, rt, sym_map, alm);
        temp_end = rt->stack.top;

        if (err_state()) {
            err_push_src(
                "EVAL",
                alm_try_get(alm, whilee->test),
                "Failed evaluating while test");
            return;
        }

        if (rt_val_peek_type(&rt->stack, test_loc) != VAL_BOOL) {
            spec_error_arg_expected("while", 1, "boolean", alm_try_get(alm, whilee->test));
            stack_collapse(&rt->stack, temp_begin, temp_end);
            return;
        }

        test_val = rt_val_peek_bool(rt, test_loc);
        if (test_val) {
            eval_dispatch(whilee->expr, rt, sym_map, alm);
            if (err_state()) {
                err_push_src(
                    "EVAL",
                    alm_try_get(alm, whilee->expr),
                    "Failed evaluating while expression");
                stack_collapse(&rt->stack, temp_begin, temp_end);
                return;
            } else {
                /* While in the loop also collapse the expression,
                 * therefre update temp end only in this case.
                 */
                temp_end = rt->stack.top;
            }
        } else {
            /* If we exit loop we leave last expresion uncollapsed */
            done = true;
        }

        stack_collapse(&rt->stack, temp_begin, temp_end);
    }
}

static void eval_special_logic(
        struct AstNode *exprs,
        struct Runtime *rt,
        struct SymMap *sym_map,
        bool breaking_value,
        char *func_name,
        struct AstLocMap *alm)
{
    int i = 0;
    VAL_LOC_T temp_begin = rt->stack.top;
    VAL_BOOL_T result = !breaking_value;

    while (exprs) {

        VAL_LOC_T loc = eval_dispatch(exprs, rt, sym_map, alm);
        if (err_state()) {
            err_push_src(
                "EVAL",
                alm_try_get(alm, exprs),
                "Failed evaluating logic special form element");
            return;
        }

        if (rt_val_peek_type(&rt->stack, loc) != VAL_BOOL) {
            spec_error_arg_expected(func_name, i, "boolean", alm_try_get(alm, exprs));
            return;
        }

        if (rt_val_peek_bool(rt, loc) == breaking_value) {
            result = breaking_value;
            break;
        }

        exprs = exprs->next;
        ++i;
    }

    stack_collapse(&rt->stack, temp_begin, rt->stack.top);
    rt_val_push_bool(&rt->stack, result);
}

static void eval_special_bind(
        struct AstNode* node,
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct AstLocMap *alm)
{
    struct AstSpecBind *bind = &node->data.special.data.bind;
    VAL_LOC_T location = eval_dispatch(bind->expr, rt, sym_map, alm);

    if (err_state()) {
        err_push_src(
            "EVAL",
            alm_try_get(alm, bind->expr),
            "Failed evaluating bind expression");
        return;
    }

    eval_special_bind_pattern(bind->pattern, location, rt, sym_map, alm);

    /* NOTE implicitly returning the bound value since there's no collapse. */
}

static void eval_special_ptr(
        struct AstNode* node,
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct AstLocMap *alm)
{
    char *symbol;
    struct SymMapNode *smn;
    struct AstSpecPtr *pointer = &node->data.special.data.pointer;

    if (pointer->expr->type != AST_SYMBOL) {
        spec_error_arg_expected("ptr", 1, "symbol", alm_try_get(alm, pointer->expr));
        return;
    }
    symbol = pointer->expr->data.symbol.symbol;
    smn = sym_map_find(sym_map, symbol);

    if (!smn) {
        eval_error_not_found_src(symbol, alm_try_get(alm, pointer->expr));
        return;
    }

    rt_val_push_ptr(&rt->stack, smn->stack_loc);
}

static void eval_special_peek(
        struct AstNode *node,
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct AstLocMap *alm)
{
    VAL_LOC_T ptr_loc, target_loc;
    VAL_LOC_T temp_begin, temp_end;
    enum ValueType ptr_type;
    struct AstSpecPeek *peek = &node->data.special.data.peek;

    temp_begin = rt->stack.top;
    ptr_loc = eval_dispatch(peek->expr, rt, sym_map, alm);
    temp_end = rt->stack.top;
    if (err_state()) {
        err_push_src(
            "EVAL",
            alm_try_get(alm, peek->expr),
            "Failed evaluating _peek_ pointer argument");
        return;
    }

    ptr_type = rt_val_peek_type(&rt->stack, ptr_loc);
    if (ptr_type != VAL_PTR) {
        spec_error_arg_expected("peek", 1, "pointer", alm_try_get(alm, peek->expr));
        return;
    }

    target_loc = rt_val_peek_ptr(rt, ptr_loc);
    rt_val_push_copy(&rt->stack, target_loc);
    stack_collapse(&rt->stack, temp_begin, temp_end);
}

static void eval_special_poke(
        struct AstNode* node,
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct AstLocMap *alm)
{
    VAL_LOC_T ptr_loc, source_loc, target_loc;
    VAL_LOC_T temp_begin, temp_end;
    enum ValueType ptr_type;
    struct AstSpecPoke *poke = &node->data.special.data.poke;

    temp_begin = rt->stack.top;
    ptr_loc = eval_dispatch(poke->pointer, rt, sym_map, alm);
    if (err_state()) {
        err_push_src(
            "EVAL",
            alm_try_get(alm, poke->pointer),
            "Failed evaluating _poke_ pointer argument");
        return;
    }

    ptr_type = rt_val_peek_type(&rt->stack, ptr_loc);
    if (ptr_type != VAL_PTR) {
        spec_error_arg_expected("poke", 1, "pointer", alm_try_get(alm, poke->pointer));
        return;
    }

    target_loc = rt_val_peek_ptr(rt, ptr_loc);

    source_loc = eval_dispatch(poke->value, rt, sym_map, alm);
    temp_end = rt->stack.top;
    if (err_state()) {
        err_push_src(
            "EVAL",
            alm_try_get(alm, poke->value),
            "Failed evaluating _poke_ source argument");
        return;
    }

    if (!rt_val_pair_homo(rt, source_loc, target_loc)) {
        err_push_src(
            "EVAL",
            alm_try_get(alm, poke->value),
            "Attempted to _poke_ value of mismatched type");
        return;
    }

    rt_val_poke_copy(&rt->stack, target_loc, source_loc);
    stack_collapse(&rt->stack, temp_begin, temp_end);
}

static void eval_special_begin(
        struct AstNode *node,
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct AstLocMap *alm)
{
    char *symbol;
    struct SymMapNode *smn;
    VAL_LOC_T cpd_loc;
    enum ValueType ptr_type;
    struct AstSpecBegin *begin = &node->data.special.data.begin;

    if (begin->collection->type != AST_SYMBOL) {
        spec_error_arg_expected(
            "begin", 1, "symbol",
            alm_try_get(alm, begin->collection));
        return;
    }
    symbol = begin->collection->data.symbol.symbol;
    smn = sym_map_find(sym_map, symbol);

    if (!smn) {
        eval_error_not_found_src(symbol, alm_try_get(alm, begin->collection));
        return;
    }

    cpd_loc = smn->stack_loc;
    ptr_type = rt_val_peek_type(&rt->stack, cpd_loc);
    if (ptr_type != VAL_ARRAY && ptr_type != VAL_TUPLE) {
        spec_error_arg_expected(
            "begin", 1, "pointer to compound object",
            alm_try_get(alm, begin->collection));
        return;
    }

    rt_val_push_ptr(&rt->stack, rt_val_cpd_first_loc(cpd_loc));
}

static void eval_special_end(
        struct AstNode *node,
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct AstLocMap *alm)
{
    int i, cpd_len;
    char *symbol;
    struct SymMapNode *smn;
    VAL_LOC_T cpd_loc;
    enum ValueType ptr_type;
    struct AstSpecEnd *end = &node->data.special.data.end;

    if (end->collection->type != AST_SYMBOL) {
        spec_error_arg_expected(
            "end", 1, "symbol",
            alm_try_get(alm, end->collection));
        return;
    }
    symbol = end->collection->data.symbol.symbol;
    smn = sym_map_find(sym_map, symbol);

    if (!smn) {
        eval_error_not_found_src(symbol, alm_try_get(alm, end->collection));
        return;
    }

    cpd_loc = smn->stack_loc;
    ptr_type = rt_val_peek_type(&rt->stack, cpd_loc);
    if (ptr_type != VAL_ARRAY && ptr_type != VAL_TUPLE) {
        spec_error_arg_expected(
        "end", 1, "pointer to compound object",
        alm_try_get(alm, end->collection));
        return;
    }

    cpd_len = rt_val_cpd_len(rt, cpd_loc);
    cpd_loc = rt_val_cpd_first_loc(cpd_loc);
    for (i = 0; i < cpd_len; ++i) {
        cpd_loc = rt_val_next_loc(rt, cpd_loc);
    }

    rt_val_push_ptr(&rt->stack, cpd_loc);
}

static void eval_special_inc(
        struct AstNode* node,
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct AstLocMap *alm)
{
    char *symbol;
    struct SymMapNode *smn;
    VAL_LOC_T ptr_loc, temp_loc;
    enum ValueType ptr_type;
    struct AstSpecInc *inc = &node->data.special.data.inc;

    if (inc->pointer->type != AST_SYMBOL) {
        spec_error_arg_expected(
            "inc", 1, "symbol",
            alm_try_get(alm, inc->pointer));
        return;
    }
    symbol = inc->pointer->data.symbol.symbol;

    smn = sym_map_find(sym_map, symbol);
    if (!smn) {
        eval_error_not_found_src(symbol, alm_try_get(alm, inc->pointer));
        return;
    }
    ptr_loc = smn->stack_loc;

    ptr_type = rt_val_peek_type(&rt->stack, ptr_loc);
    if (ptr_type != VAL_PTR) {
        spec_error_arg_expected(
            "inc", 1, "pointer to pointer",
            alm_try_get(alm, inc->pointer));
        return;
    }

    temp_loc = rt_val_peek_ptr(rt, ptr_loc);
    temp_loc = rt_val_next_loc(rt, temp_loc);
    rt_val_poke_ptr(&rt->stack, ptr_loc, temp_loc);

    rt_val_push_unit(&rt->stack);
}

static void eval_special_succ(
        struct AstNode* node,
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct AstLocMap *alm)
{
    VAL_LOC_T temp_begin, temp_end;
    VAL_LOC_T ptr_loc, target_loc;
    enum ValueType ptr_type;
    struct AstSpecSucc *succ = &node->data.special.data.succ;

    temp_begin = rt->stack.top;
    ptr_loc = eval_dispatch(succ->pointer, rt, sym_map, alm);
    temp_end = rt->stack.top;
    if (err_state()) {
        err_push_src(
            "EVAL",
            alm_try_get(alm, succ->pointer),
            "Failed evaluating _succ_ pointer argument");
        return;
    }

    ptr_type = rt_val_peek_type(&rt->stack, ptr_loc);
    if (ptr_type != VAL_PTR) {
        spec_error_arg_expected(
            "succ", 1, "pointer",
            alm_try_get(alm, succ->pointer));
        return;
    }

    target_loc = rt_val_peek_ptr(rt, ptr_loc);
    target_loc = rt_val_next_loc(rt, target_loc);
    rt_val_push_ptr(&rt->stack, target_loc);
    stack_collapse(&rt->stack, temp_begin, temp_end);
}

void eval_special(
        struct AstNode *node,
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct AstLocMap *alm)
{
    struct AstSpecial *special = &node->data.special;
    enum AstSpecialType type = special->type;

    switch (type) {
    case AST_SPEC_DO:
        eval_special_do(node, rt, sym_map, alm);
        break;

    case AST_SPEC_MATCH:
        eval_special_match(node, rt, sym_map, alm);
        break;

    case AST_SPEC_IF:
        eval_special_if(node, rt, sym_map, alm);
        break;

    case AST_SPEC_WHILE:
        eval_special_while(node, rt, sym_map, alm);
        break;

    case AST_SPEC_FUNC_DEF:
        eval_special_func_def(node, rt, sym_map, alm);
        break;

    case AST_SPEC_BOOL_AND:
        eval_special_logic(
            node->data.special.data.bool_and.exprs,
            rt, sym_map, false, "and", alm);
        break;

    case AST_SPEC_BOOL_OR:
        eval_special_logic(
            node->data.special.data.bool_or.exprs,
            rt, sym_map, true, "or", alm);
        break;

    case AST_SPEC_SET_OF:
        eval_special_set_of(node, rt, sym_map, alm);
        break;

    case AST_SPEC_RANGE_OF:
        eval_special_range_of(node, rt, sym_map, alm);
        break;

    case AST_SPEC_ARRAY_OF:
        eval_special_array_of(node, rt, sym_map, alm);
        break;

    case AST_SPEC_TUPLE_OF:
        eval_special_tuple_of(node, rt, sym_map, alm);
        break;

    case AST_SPEC_POINTER_TO:
        eval_special_pointer_to(node, rt, sym_map, alm);
        break;

    case AST_SPEC_FUNCTION_TYPE:
        eval_special_function_type(node, rt, sym_map, alm);
        break;

    case AST_SPEC_TYPE_PRODUCT:
        eval_special_type_op(node, rt, sym_map, alm, true);
        break;

    case AST_SPEC_TYPE_UNION:
        eval_special_type_op(node, rt, sym_map, alm, false);
        break;

    case AST_SPEC_BIND:
        eval_special_bind(node, rt, sym_map, alm);
        break;

    case AST_SPEC_PTR:
        eval_special_ptr(node, rt, sym_map, alm);
        break;

    case AST_SPEC_PEEK:
        eval_special_peek(node, rt, sym_map, alm);
        break;

    case AST_SPEC_POKE:
        eval_special_poke(node, rt, sym_map, alm);
        break;

    case AST_SPEC_BEGIN:
        eval_special_begin(node, rt, sym_map, alm);
        break;

    case AST_SPEC_END:
        eval_special_end(node, rt, sym_map, alm);
        break;

    case AST_SPEC_INC:
        eval_special_inc(node, rt, sym_map, alm);
        break;

    case AST_SPEC_SUCC:
        eval_special_succ(node, rt, sym_map, alm);
        break;
    }
}

