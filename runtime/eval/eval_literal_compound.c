/* Copyright (C) 2015 Krzysztof Stachowiak */

#include "error.h"
#include "rt_val.h"

#include "eval_detail.h"
#include "eval.h"

void eval_literal_compound(
        struct AstLiteralCompound *literal_compound,
        struct Runtime *rt,
        struct SymMap *sym_map)
{
    VAL_LOC_T size_loc = -1, data_begin, data_size;
    struct AstNode *current = literal_compound->exprs;
    bool has_first_elem = false;
    VAL_LOC_T first_elem_loc, elem_loc;

    /* Header. */
    switch (literal_compound->type) {
    case AST_LIT_CPD_ARRAY:
        rt_val_push_array_init(&rt->stack, &size_loc);
        break;

    case AST_LIT_CPD_TUPLE:
        rt_val_push_tuple_init(&rt->stack, &size_loc);
        break;
    }

    /* Data. */
    data_begin = rt->stack.top;
    while (current) {
        elem_loc = eval_dispatch(current, rt, sym_map);
        if (err_state()) {
            err_push_src("EVAL", current->loc, "Failed evaluating literal compound expression element");
            return;
        } else {
            current = current->next;
        }

        if (literal_compound->type != AST_LIT_CPD_ARRAY) {
            continue;
        }

        /* Homogenity check */
        if (!has_first_elem) {
            has_first_elem = true;
            first_elem_loc = elem_loc;
        } else if (!rt_val_pair_homo(rt, first_elem_loc, elem_loc)) {
            err_push("EVAL", "Heterogenous array literal evaluated");
            return;
        }
    }

    /* Hack value size to correct value. */
    data_size = rt->stack.top - data_begin;
    rt_val_push_cpd_final(&rt->stack, size_loc, data_size);
}

