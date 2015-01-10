/* Copyright (C) 2015 Krzysztof Stachowiak */

#include "error.h"
#include "rt_val.h"

#include "eval_detail.h"
#include "eval.h"

void eval_compound(
        struct AstNode *node,
        struct Runtime *rt,
        struct SymMap *sym_map)
{
    VAL_LOC_T size_loc, data_begin, data_size;
    struct AstNode *current = node->data.compound.exprs;

	/* Header. */
    switch (node->data.compound.type) {
        case AST_CPD_ARRAY:
			rt_val_push_array_init(rt->stack, &size_loc);
            break;

        case AST_CPD_TUPLE:
			rt_val_push_tuple_init(rt->stack, &size_loc);
            break;
    }

    /* Data. */
    data_begin = rt->stack->top;
    while (current) {
        eval_impl(current, rt, sym_map);
        if (err_state()) {
            return;
        }
        current = current->next;
    }

    /* Hack value size to correct value. */
    data_size = rt->stack->top - data_begin;
	rt_val_push_cpd_final(rt->stack, size_loc, data_size);
}

