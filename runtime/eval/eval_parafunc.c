/* Copyright (C) 2015 Krzysztof Stachowiak */

#include <stdbool.h>

#include "error.h"
#include "eval.h"
#include "eval_detail.h"
#include "rt_val.h"

static void para_logic_error_no_args(void)
{
	struct ErrMessage msg;
	err_msg_init_src(&msg, "EVAL PARAFUNC LOGIC", eval_location_top());
	err_msg_append(&msg, "Passed no arguments to logic parafunc");
	err_msg_set(&msg);
}

static void para_logic_error_arg_not_bool(int index)
{
	struct ErrMessage msg;
	err_msg_init_src(&msg, "EVAL PARAFUNC LOGIC", eval_location_top());
	err_msg_append(&msg, "argument %d is not of boolean type", index);
	err_msg_set(&msg);
}

static void eval_parafunc_logic(
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct AstNode *args,
        bool breaking_value)
{
    int i = 0;
    VAL_LOC_T temp_begin = rt->stack->top;
    VAL_BOOL_T result = !breaking_value;

    if (!args) {
        para_logic_error_no_args();
        return;
    }

    while (args) {

        VAL_LOC_T loc = eval_impl(args, rt, sym_map);
        if (err_state()) {
            return;
        }

        if (rt_val_peek_type(rt, loc) != VAL_BOOL) {
            para_logic_error_arg_not_bool(i);
            return;
        }

        if (rt_val_peek_bool(rt, loc) == breaking_value) {
            result = breaking_value;
            break;
        }

        args = args->next;
        ++i;
    }

    stack_collapse(rt->stack, temp_begin, rt->stack->top);
    rt_val_push_bool(rt->stack, result);
}

void eval_parafunc(struct AstNode *node, struct Runtime *rt, struct SymMap *sym_map)
{
    enum AstParafuncType type = node->data.parafunc.type;
    struct AstNode *args = node->data.parafunc.args;

    switch (type) {
    case AST_PARAFUNC_AND:
        eval_parafunc_logic(rt, sym_map, args, false);
        break;

    case AST_PARAFUNC_OR:
        eval_parafunc_logic(rt, sym_map, args, true);
        break;
    }
}
