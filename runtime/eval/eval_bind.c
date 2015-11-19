/* Copyright (C) 2015 Krzysztof Stachowiak */

#include "error.h"
#include "runtime.h"
#include "rt_val.h"
#include "eval.h"
#include "eval_detail.h"

void eval_bind(
        struct AstNode *node,
        struct Runtime *rt,
        struct SymMap *sym_map)
{
    struct Pattern *pattern = node->data.bind.pattern;
    struct AstNode *expr = node->data.bind.expr;

    VAL_LOC_T location = eval_impl(expr, rt, sym_map);
    if (err_state()) {
        err_push_src("EVAL", expr->loc, "Failed evaluating bind expression");
        return;
    }

    eval_bind_pattern(rt, sym_map, pattern, location, &expr->loc);

    /* NOTE implicitly returning the bound value since there's no collapse. */
}

