/* Copyright (C) 2015 Krzysztof Stachowiak */

#include "runtime.h"
#include "error.h"
#include "eval_detail.h"

void eval_parafunc_logic(
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct AstNode *args,
        bool breaking_value,
        char *func)
{
    int i = 0;
    VAL_LOC_T temp_begin = rt->stack.top;
    VAL_BOOL_T result = !breaking_value;

    if (!args) {
        para_error_invalid_argc(func, 0);
        return;
    }

    while (args) {

        VAL_LOC_T loc = eval_dispatch(args, rt, sym_map);
        if (err_state()) {
            err_push_src("EVAL", args->loc, "Failed evaluating logic parafunc element");
            return;
        }

        if (rt_val_peek_type(&rt->stack, loc) != VAL_BOOL) {
            para_error_arg_expected(func, i, "boolean");
            return;
        }

        if (rt_val_peek_bool(rt, loc) == breaking_value) {
            result = breaking_value;
            break;
        }

        args = args->next;
        ++i;
    }

    stack_collapse(&rt->stack, temp_begin, rt->stack.top);
    rt_val_push_bool(&rt->stack, result);
}

