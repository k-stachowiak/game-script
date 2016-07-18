/* Copyright (C) 2015 Krzysztof Stachowiak */

#include <stdbool.h>
#include <inttypes.h>

#include "error.h"
#include "eval.h"
#include "eval_detail.h"
#include "rt_val.h"

void spec_error_invalid_argc(char *func, int count)
{
    err_push("EVAL",
        "Incorrect arguments count passed to _%s_: %d",
        func, count);
}

void spec_error_arg_expected(char *func, int index, char *expected)
{
    err_push("EVAL",
            "Argument %d of special form _%s_ must be %s",
            index, func, expected);
}

static void eval_special_do(
        struct AstSpecDo *doo,
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct SourceLocation *src_loc)
{
    struct SymMap local_sym_map;
    struct AstNode *expr = doo->exprs;

    (void)src_loc;

    sym_map_init_local(&local_sym_map, sym_map);

    VAL_LOC_T begin = rt->stack.top;
    VAL_LOC_T end = rt->stack.top;

    for (; expr; expr = expr->next) {
        VAL_LOC_T new_end = eval_dispatch(expr, rt, &local_sym_map);
        if (err_state()) {
            err_push_src("EVAL", expr->loc, "Failed evaluating do expression");
            break;
        } else {
            end = new_end;
        }
    }

    stack_collapse(&rt->stack, begin, end);

    sym_map_deinit(&local_sym_map);
}

static void eval_special_match(
        struct AstSpecMatch *match,
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct SourceLocation *src_loc)
{
    struct AstNode *expr = match->expr;
    struct Pattern *key = match->keys;
    struct AstNode *value = match->values;
    VAL_LOC_T temp_begin, temp_end;

    temp_begin = rt->stack.top;
    VAL_LOC_T location = eval_dispatch(expr, rt, sym_map);
    if (err_state()) {
        err_push_src("EVAL", expr->loc, "Failed evaluating match expression");
        return;
    }
    temp_end = rt->stack.top;

    while (key && value) {
        struct SymMap local_sym_map;
        sym_map_init_local(&local_sym_map, sym_map);

        eval_special_bind_pattern(key, location, rt, &local_sym_map, &expr->loc);

        if (err_state()) {
            err_reset(); /* We ignore the error while in this loop. */
            key = key->next;
            value = value->next;
        } else {
            eval_dispatch(value, rt, &local_sym_map);
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
        "EVAL", *src_loc,
        "None of the cases were matched in match expression");

end:
    stack_collapse(&rt->stack, temp_begin, temp_end);
}

static void eval_special_if(
        struct AstSpecIf* iff,
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct SourceLocation *src_loc)
{
    VAL_LOC_T test_loc, temp_begin, temp_end;
    VAL_BOOL_T test_val;

    temp_begin = rt->stack.top;
    test_loc = eval_dispatch(iff->test, rt, sym_map);
    temp_end = rt->stack.top;

    if (err_state()) {
        err_push_src("EVAL", *src_loc, "Failed evaluating if test");
        return;
    }

    if (rt_val_peek_type(&rt->stack, test_loc) != VAL_BOOL) {
        spec_error_arg_expected("if", 1, "boolean");
        stack_collapse(&rt->stack, temp_begin, temp_end);
        return;
    }

    test_val = rt_val_peek_bool(rt, test_loc);
    stack_collapse(&rt->stack, temp_begin, temp_end);

    if (test_val) {
        eval_dispatch(iff->true_expr, rt, sym_map);
    } else {
        eval_dispatch(iff->false_expr, rt, sym_map);
    }
}

static void eval_special_while(
        struct AstSpecWhile* whilee,
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct SourceLocation *src_loc)
{
    bool done = false;

    while (!done) {

        VAL_LOC_T test_loc, temp_begin, temp_end;
        VAL_BOOL_T test_val;

        temp_begin = rt->stack.top;
        test_loc = eval_dispatch(whilee->test, rt, sym_map);
        temp_end = rt->stack.top;

        if (err_state()) {
            err_push_src("EVAL", *src_loc, "Failed evaluating while test");
            return;
        }

        if (rt_val_peek_type(&rt->stack, test_loc) != VAL_BOOL) {
            spec_error_arg_expected("while", 1, "boolean");
            stack_collapse(&rt->stack, temp_begin, temp_end);
            return;
        }

        test_val = rt_val_peek_bool(rt, test_loc);
        if (test_val) {
            eval_dispatch(whilee->expr, rt, sym_map);
            if (err_state()) {
                err_push_src("EVAL", whilee->test->loc, "Failed evaluating while expression");
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
        char *func_name)
{
    int i = 0;
    VAL_LOC_T temp_begin = rt->stack.top;
    VAL_BOOL_T result = !breaking_value;

    while (exprs) {

        VAL_LOC_T loc = eval_dispatch(exprs, rt, sym_map);
        if (err_state()) {
            err_push_src("EVAL", exprs->loc, "Failed evaluating logic special form element");
            return;
        }

        if (rt_val_peek_type(&rt->stack, loc) != VAL_BOOL) {
            spec_error_arg_expected(func_name, i, "boolean");
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
        struct AstSpecBind* bind,
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct SourceLocation *src_loc)
{
    VAL_LOC_T location = eval_dispatch(bind->expr, rt, sym_map);

    (void)src_loc;

    if (err_state()) {
        err_push_src("EVAL", bind->expr->loc, "Failed evaluating bind expression");
        return;
    }

    eval_special_bind_pattern(bind->pattern, location, rt, sym_map, &bind->expr->loc);

    /* NOTE implicitly returning the bound value since there's no collapse. */
}

static void eval_special_ref(
        struct AstSpecRef* ref,
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct SourceLocation *src_loc)
{
    char *symbol;
    struct SymMapNode *smn;

    (void)src_loc;

    if (ref->expr->type != AST_SYMBOL) {
        spec_error_arg_expected("ref", 1, "symbol");
        return;
    }
    symbol = ref->expr->data.symbol.symbol;
    smn = sym_map_find(sym_map, symbol);

    if (!smn) {
        eval_error_not_found(symbol);
        return;
    }

    rt_val_push_ref(&rt->stack, smn->stack_loc);
}

static void eval_special_peek(
        struct AstSpecPeek* peek,
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct SourceLocation *src_loc)
{
    VAL_LOC_T ref_loc, target_loc;
    VAL_LOC_T temp_begin, temp_end;
    enum ValueType ref_type;

    (void)src_loc;

    temp_begin = rt->stack.top;
    ref_loc = eval_dispatch(peek->expr, rt, sym_map);
    temp_end = rt->stack.top;
    if (err_state()) {
        err_push_src("EVAL", peek->expr->loc, "Failed evaluating _peek_ reference argument");
        return;
    }

    ref_type = rt_val_peek_type(&rt->stack, ref_loc);
    if (ref_type != VAL_REF) {
        spec_error_arg_expected("peek", 1, "reference");
        return;
    }

    target_loc = rt_val_peek_ref(rt, ref_loc);
    rt_val_push_copy(&rt->stack, target_loc);
    stack_collapse(&rt->stack, temp_begin, temp_end);
}

static void eval_special_poke(
        struct AstSpecPoke* poke,
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct SourceLocation *src_loc)
{
    VAL_LOC_T ref_loc, source_loc, target_loc;
    VAL_LOC_T temp_begin, temp_end;
    enum ValueType ref_type;

    (void)src_loc;

    temp_begin = rt->stack.top;
    ref_loc = eval_dispatch(poke->reference, rt, sym_map);
    if (err_state()) {
        err_push_src("EVAL", poke->reference->loc, "Failed evaluating _poke_ reference argument");
        return;
    }

    ref_type = rt_val_peek_type(&rt->stack, ref_loc);
    if (ref_type != VAL_REF) {
        spec_error_arg_expected("poke", 1, "reference");
        return;
    }

    target_loc = rt_val_peek_ref(rt, ref_loc);

    source_loc = eval_dispatch(poke->value, rt, sym_map);
    temp_end = rt->stack.top;
    if (err_state()) {
        err_push_src("EVAL", poke->value->loc, "Failed evaluating _poke_ source argument");
        return;
    }

    if (!rt_val_pair_homo(rt, source_loc, target_loc)) {
        err_push_src("EVAL", poke->value->loc, "Attempted to _poke_ value of mismatched type");
        return;
    }

    rt_val_poke_copy(&rt->stack, target_loc, source_loc);
    stack_collapse(&rt->stack, temp_begin, temp_end);
}

static void eval_special_begin(
        struct AstSpecBegin* begin,
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct SourceLocation *src_loc)
{
    char *symbol;
    struct SymMapNode *smn;
    VAL_LOC_T cpd_loc;
    enum ValueType ref_type;

    (void)src_loc;

    if (begin->collection->type != AST_SYMBOL) {
        spec_error_arg_expected("begin", 1, "symbol");
        return;
    }
    symbol = begin->collection->data.symbol.symbol;
    smn = sym_map_find(sym_map, symbol);

    if (!smn) {
        eval_error_not_found(symbol);
        return;
    }

    cpd_loc = smn->stack_loc;
    ref_type = rt_val_peek_type(&rt->stack, cpd_loc);
    if (ref_type != VAL_ARRAY && ref_type != VAL_TUPLE) {
        spec_error_arg_expected("begin", 1, "reference to compound object");
        return;
    }

    rt_val_push_ref(&rt->stack, rt_val_cpd_first_loc(cpd_loc));
}

static void eval_special_end(
        struct AstSpecEnd* end,
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct SourceLocation *src_loc)
{
    int i, cpd_len;
    char *symbol;
    struct SymMapNode *smn;
    VAL_LOC_T cpd_loc;
    enum ValueType ref_type;

    (void)src_loc;

    if (end->collection->type != AST_SYMBOL) {
        spec_error_arg_expected("end", 1, "symbol");
        return;
    }
    symbol = end->collection->data.symbol.symbol;
    smn = sym_map_find(sym_map, symbol);

    if (!smn) {
        eval_error_not_found(symbol);
        return;
    }

    cpd_loc = smn->stack_loc;
    ref_type = rt_val_peek_type(&rt->stack, cpd_loc);
    if (ref_type != VAL_ARRAY && ref_type != VAL_TUPLE) {
        spec_error_arg_expected("end", 1, "reference to compound object");
        return;
    }

    cpd_len = rt_val_cpd_len(rt, cpd_loc);
    cpd_loc = rt_val_cpd_first_loc(cpd_loc);
    for (i = 0; i < cpd_len; ++i) {
        cpd_loc = rt_val_next_loc(rt, cpd_loc);
    }

    rt_val_push_ref(&rt->stack, cpd_loc);
}

static void eval_special_inc(
        struct AstSpecInc* inc,
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct SourceLocation *src_loc)
{
    char *symbol;
    struct SymMapNode *smn;
    VAL_LOC_T ref_loc, temp_loc;
    enum ValueType ref_type;

    (void)src_loc;

    if (inc->reference->type != AST_SYMBOL) {
        spec_error_arg_expected("inc", 1, "symbol");
        return;
    }
    symbol = inc->reference->data.symbol.symbol;

    smn = sym_map_find(sym_map, symbol);
    if (!smn) {
        eval_error_not_found(symbol);
        return;
    }
    ref_loc = smn->stack_loc;

    ref_type = rt_val_peek_type(&rt->stack, ref_loc);
    if (ref_type != VAL_REF) {
        spec_error_arg_expected("inc", 1, "reference to reference");
        return;
    }

    temp_loc = rt_val_peek_ref(rt, ref_loc);
    temp_loc = rt_val_next_loc(rt, temp_loc);
    rt_val_poke_ref(&rt->stack, ref_loc, temp_loc);

    rt_val_push_unit(&rt->stack);
}

static void eval_special_succ(
        struct AstSpecSucc* succ,
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct SourceLocation *src_loc)
{
    VAL_LOC_T temp_begin, temp_end;
    VAL_LOC_T ref_loc, target_loc;
    enum ValueType ref_type;

    (void)src_loc;

    temp_begin = rt->stack.top;
    ref_loc = eval_dispatch(succ->reference, rt, sym_map);
    temp_end = rt->stack.top;
    if (err_state()) {
        err_push_src("EVAL", succ->reference->loc, "Failed evaluating _succ_ reference argument");
        return;
    }

    ref_type = rt_val_peek_type(&rt->stack, ref_loc);
    if (ref_type != VAL_REF) {
        spec_error_arg_expected("succ", 1, "reference");
        return;
    }

    target_loc = rt_val_peek_ref(rt, ref_loc);
    target_loc = rt_val_next_loc(rt, target_loc);
    rt_val_push_ref(&rt->stack, target_loc);
    stack_collapse(&rt->stack, temp_begin, temp_end);
}

void eval_special(
        struct AstSpecial *special,
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct SourceLocation *src_loc)
{
    enum AstSpecialType type = special->type;

    switch (type) {
    case AST_SPEC_DO:
        eval_special_do(&special->data.doo, rt, sym_map, src_loc);
        break;

    case AST_SPEC_MATCH:
        eval_special_match(&special->data.match, rt, sym_map, src_loc);
        break;

    case AST_SPEC_IF:
        eval_special_if(&special->data.iff, rt, sym_map, src_loc);
        break;

    case AST_SPEC_WHILE:
        eval_special_while(&special->data.whilee, rt, sym_map, src_loc);
        break;

    case AST_SPEC_FUNC_DEF:
        eval_special_func_def(&special->data.func_def, rt, sym_map, src_loc);
        break;

    case AST_SPEC_AND:
        eval_special_logic(special->data.andd.exprs, rt, sym_map, false, "and");
        break;

    case AST_SPEC_OR:
        eval_special_logic(special->data.orr.exprs, rt, sym_map, true, "or");
        break;

    case AST_SPEC_BIND:
        eval_special_bind(&special->data.bind, rt, sym_map, src_loc);
        break;

    case AST_SPEC_REF:
        eval_special_ref(&special->data.ref, rt, sym_map, src_loc);
        break;

    case AST_SPEC_PEEK:
        eval_special_peek(&special->data.peek, rt, sym_map, src_loc);
        break;

    case AST_SPEC_POKE:
        eval_special_poke(&special->data.poke, rt, sym_map, src_loc);
        break;

    case AST_SPEC_BEGIN:
        eval_special_begin(&special->data.begin, rt, sym_map, src_loc);
        break;

    case AST_SPEC_END:
        eval_special_end(&special->data.end, rt, sym_map, src_loc);
        break;

    case AST_SPEC_INC:
        eval_special_inc(&special->data.inc, rt, sym_map, src_loc);
        break;

    case AST_SPEC_SUCC:
        eval_special_succ(&special->data.succ, rt, sym_map, src_loc);
        break;
    }
}
