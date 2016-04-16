/* Copyright (C) 2015 Krzysztof Stachowiak */

#include "error.h"
#include "runtime.h"
#include "symmap.h"
#include "ast.h"
#include "eval_detail.h"

void eval_special_if(
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct AstNode *args)
{
    VAL_LOC_T test_loc, temp_begin, temp_end;
    VAL_BOOL_T test_val;
    int argc = ast_list_len(args);

    if (argc != 3) {
        spec_error_invalid_argc("if", argc);
        return;
    }

    temp_begin = rt->stack.top;
    test_loc = eval_dispatch(args, rt, sym_map);
    temp_end = rt->stack.top;

    if (err_state()) {
        err_push_src("EVAL", args->loc, "Failed evaluating if test");
        return;
    }

    if (rt_val_peek_type(&rt->stack, test_loc) != VAL_BOOL) {
        spec_error_arg_expected("if", 1, "boolean");
        stack_collapse(&rt->stack, temp_begin, temp_end);
        return;
    }

    test_val = rt_val_peek_bool(rt, test_loc);
    stack_collapse(&rt->stack, temp_begin, temp_end);

    if (test_val) {
        eval_dispatch(args->next, rt, sym_map);
    } else {
        eval_dispatch(args->next->next, rt, sym_map);
    }
}

void eval_special_while(
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct AstNode *args)
{
    int argc = ast_list_len(args);
    bool done = false;

    if (argc != 2) {
        spec_error_invalid_argc("while", argc);
        return;
    }

    while (!done) {

        VAL_LOC_T test_loc, temp_begin, temp_end;
        VAL_BOOL_T test_val;

        temp_begin = rt->stack.top;
        test_loc = eval_dispatch(args, rt, sym_map);
        temp_end = rt->stack.top;

        if (err_state()) {
            err_push_src("EVAL", args->loc, "Failed evaluating while test");
            return;
        }

        if (rt_val_peek_type(&rt->stack, test_loc) != VAL_BOOL) {
            spec_error_arg_expected("while", 1, "boolean");
            stack_collapse(&rt->stack, temp_begin, temp_end);
            return;
        }

        test_val = rt_val_peek_bool(rt, test_loc);
        if (test_val) {
            eval_dispatch(args->next, rt, sym_map);
            if (err_state()) {
                err_push_src("EVAL", args->loc, "Failed evaluating while expression");
                stack_collapse(&rt->stack, temp_begin, temp_end);
                return;
            } else {
                /* While in the loop also collapse the expression,
                 * therefre update temp end only in this case.
                 */
                temp_end = rt->stack.top;
            }
        } else {
            /* If we exit loop we leave last expresion uncollapsed */
            done = true;
        }

        stack_collapse(&rt->stack, temp_begin, temp_end);
    }
}

