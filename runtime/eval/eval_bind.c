/* Copyright (C) 2015 Krzysztof Stachowiak */

#include "error.h"
#include "runtime.h"
#include "rt_val.h"
#include "eval.h"
#include "eval_detail.h"

void eval_bind_pattern(
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct Pattern *pattern,
        VAL_LOC_T location,
        struct SourceLocation *source_loc)
{
    VAL_LOC_T child_loc;
    struct Pattern *child_pat;
    int i, cpd_len, pattern_len;
    enum ValueType type = rt_val_peek_type(&rt->stack, location);

    if (pattern->type == PATTERN_DONTCARE) {
        return;
    }

    if (pattern->type == PATTERN_SYMBOL) {
        sym_map_insert(
            sym_map,
            pattern->data.symbol.symbol,
            location,
            *source_loc);
        return;
    }

    if ((pattern->type == PATTERN_ARRAY && type != VAL_ARRAY) ||
        (pattern->type == PATTERN_TUPLE && type != VAL_TUPLE)) {
        err_push("EVAL", "Compound type mismatched");
        return;
    }

    cpd_len = rt_val_cpd_len(rt, location);
    pattern_len = pattern_list_len(pattern->data.compound.children);
    if (cpd_len != pattern_len) {
        err_push("EVAL", "Compound bind length mismatched");
        return;
    }

    child_loc = rt_val_cpd_first_loc(location);
    child_pat = pattern->data.compound.children;
    for (i = 0; i < pattern_len; ++i) {
        eval_bind_pattern(rt, sym_map, child_pat, child_loc, source_loc);
        if (err_state()) {
            err_push("EVAL", "Failed evaluating bind pattern");
            return;
        }
        child_loc = rt_val_next_loc(rt, child_loc);
        child_pat = child_pat->next;
    }
}

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

