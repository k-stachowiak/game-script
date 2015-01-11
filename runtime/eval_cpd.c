/* Copyright (C) 2015 Krzysztof Stachowiak */

#include "error.h"
#include "rt_val.h"

#include "eval_detail.h"
#include "eval.h"

static void eval_error_array_hetero(void)
{
	struct ErrMessage msg;
	err_msg_init_src(&msg, "EVAL", eval_location_top());
	err_msg_append(&msg, "Heterogenous array literal evaluated");
	err_msg_set(&msg);
}

static bool eval_pair_homo(struct Runtime *rt, VAL_LOC_T x, VAL_LOC_T y);

static bool eval_pair_homo_simple(struct Runtime *rt, VAL_LOC_T x, VAL_LOC_T y)
{
    struct ValueHeader
        header_x = rt_val_peek_header(rt->stack, x),
        header_y = rt_val_peek_header(rt->stack, y);

    return (
        (header_x.type == VAL_BOOL && header_y.type == VAL_BOOL) ||
        (header_x.type == VAL_CHAR && header_y.type == VAL_CHAR) ||
        (header_x.type == VAL_INT && header_y.type == VAL_INT) ||
        (header_x.type == VAL_REAL && header_y.type == VAL_REAL) ||
        (header_x.type == VAL_STRING && header_y.type == VAL_STRING) ||
        (header_x.type == VAL_FUNCTION && header_y.type == VAL_FUNCTION)
    );
}

static bool eval_pair_homo_complex(struct Runtime *rt, VAL_LOC_T x, VAL_LOC_T y)
{
    VAL_LOC_T current_x, current_y, last_x;

    struct ValueHeader
        header_x = rt_val_peek_header(rt->stack, x),
        header_y = rt_val_peek_header(rt->stack, y);

    if (!(header_x.type == VAL_ARRAY && header_y.type == VAL_ARRAY) &&
        !(header_x.type == VAL_TUPLE && header_y.type == VAL_TUPLE)) {
        return false;
    }

    if (rt_val_cpd_len(rt, x) != rt_val_cpd_len(rt, y)) {
        return false;
    }

    current_x = rt_val_cpd_first_loc(x);
    current_y = rt_val_cpd_first_loc(y);
    last_x = current_x + rt_val_peek_size(rt, current_x);

    while (current_x != last_x) {

        if (!eval_pair_homo(rt, current_x, current_y)) {
            return false;
        }

        current_x = rt_val_next_loc(rt, current_x);
        current_y = rt_val_next_loc(rt, current_y);
    }

    return true;
}

static bool eval_pair_homo(struct Runtime *rt, VAL_LOC_T x, VAL_LOC_T y)
{
    return eval_pair_homo_simple(rt, x, y) ||
           eval_pair_homo_complex(rt, x, y);
}

static bool eval_compound_homo(struct Runtime *rt, VAL_LOC_T val_loc)
{
    int i, len = rt_val_cpd_len(rt, val_loc);
    VAL_LOC_T first, current;

    if (len < 2) {
        return true;
    }

    first = rt_val_cpd_first_loc(val_loc);
    current = rt_val_next_loc(rt, first);

    for (i = 1; i < len; ++i) {
        if (!eval_pair_homo(rt, first, current)) {
            return false;
        }
    }
    
    return true;
}

void eval_compound(
        struct AstNode *node,
        struct Runtime *rt,
        struct SymMap *sym_map)
{
    VAL_LOC_T size_loc, data_begin, data_size;
    struct AstNode *current = node->data.compound.exprs;
    VAL_LOC_T result_loc = rt->stack->top;

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

    /* Assert array homogenity. */
    if (node->data.compound.type == AST_CPD_ARRAY &&
        eval_compound_homo(rt, result_loc) == false) {
        eval_error_array_hetero();
    }
}

