/* Copyright (C) 2014,2015 Krzysztof Stachowiak */

#include "error.h"
#include "runtime.h"
#include "eval.h"
#include "eval_detail.h"

static void eval_do(
        struct AstCtlDo *doo,
        struct Runtime *rt,
        struct SymMap *sym_map)
{
    struct SymMap local_sym_map;
    struct AstNode *expr = doo->exprs;

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

static void eval_bind(
        struct AstCtlBind *bind,
        struct Runtime *rt,
        struct SymMap *sym_map)
{
    struct Pattern *pattern = bind->pattern;
    struct AstNode *expr = bind->expr;

    VAL_LOC_T location = eval_dispatch(expr, rt, sym_map);
    if (err_state()) {
        err_push_src("EVAL", expr->loc, "Failed evaluating bind expression");
        return;
    }

    eval_bind_pattern(pattern, rt, sym_map, location, &expr->loc);

    /* NOTE implicitly returning the bound value since there's no collapse. */
}

static void eval_match(
        struct AstCtlMatch *match,
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

        eval_bind_pattern(key, rt, &local_sym_map, location, &expr->loc);

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

static void eval_reference(
        struct AstCtlReference *reference,
        struct Runtime *rt,
        struct SymMap *sym_map)
{
    struct SymMapNode *smn;
    char *symbol = reference->symbol;

    LOG_TRACE_FUNC;

    if (!(smn = sym_map_find(sym_map, symbol))) {
        eval_error_not_found(symbol);
        return;
    }

    rt_val_push_copy(&rt->stack, smn->stack_loc);
}

void eval_control(
        struct AstControl *control,
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct SourceLocation *src_loc)
{
    switch (control->type)
    {
    case AST_CTL_DO:
        eval_do(&control->data.doo, rt, sym_map);
        break;

    case AST_CTL_BIND:
        eval_bind(&control->data.bind, rt, sym_map);
        break;

    case AST_CTL_MATCH:
        eval_match(&control->data.match, rt, sym_map, src_loc);
        break;

    case AST_CTL_FUNC_DEF:
        eval_func_def(&control->data.fdef, rt, sym_map);
        break;

    case AST_CTL_FUNC_CALL:
        eval_func_call(&control->data.fcall, rt, sym_map);
        break;

    case AST_CTL_REFERENCE:
        eval_reference(&control->data.reference, rt, sym_map);
        break;
    }
}

