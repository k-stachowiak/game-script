/* Copyright (C) 2015 Krzysztof Stachowiak */

#include "runtime.h"
#include "error.h"

#include "eval_detail.h"

void eval_do_block(
        struct AstNode *node,
        struct Runtime *rt,
        struct SymMap *sym_map)
{
    struct SymMap local_sym_map;
    struct AstNode *expr = node->data.do_block.exprs;

    sym_map_init_local(&local_sym_map, sym_map);

    VAL_LOC_T begin = rt->stack->top;
    VAL_LOC_T end = rt->stack->top;

    for (; expr; expr = expr->next) {
        VAL_LOC_T new_end = eval_impl(expr, rt, &local_sym_map);
        if (err_state()) {
            break;
        } else {
            end = new_end;
        }
    }

    stack_collapse(rt->stack, begin, end);

    sym_map_deinit(&local_sym_map);
}

