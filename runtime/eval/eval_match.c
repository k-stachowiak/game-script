/* Copyright (C) 2015 Krzysztof Stachowiak */

#include "error.h"
#include "runtime.h"
#include "rt_val.h"
#include "eval.h"
#include "eval_detail.h"

void eval_match(
        struct AstNode *node,
        struct Runtime *rt,
        struct SymMap *sym_map)
{
    struct AstNode *expr = node->data.match.expr;
    struct Pattern *key = node->data.match.keys;
    struct AstNode *value = node->data.match.values;
    VAL_LOC_T temp_begin, temp_end;

    temp_begin = rt->stack.top;
    VAL_LOC_T location = eval_impl(expr, rt, sym_map);
    if (err_state()) {
        err_push_src("EVAL", expr->loc, "Failed evaluating match expression");
        return;
    }
    temp_end = rt->stack.top;

    while (key && value) {
        struct SymMap local_sym_map;
        sym_map_init_local(&local_sym_map, sym_map);

        eval_bind_pattern(rt, &local_sym_map, key, location, &expr->loc);

        if (err_state()) {
            err_reset(); /* We ignore the error while in this loop. */
            key = key->next;
            value = value->next;
        } else {
            eval_impl(value, rt, &local_sym_map);
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
        "EVAL", node->loc,
        "None of the cases were matched in match expression");

end:
    stack_collapse(&rt->stack, temp_begin, temp_end);
}
