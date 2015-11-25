/* Copyright (C) 2015 Krzysztof Stachowiak */

#include <stdbool.h>
#include <inttypes.h>

#include "error.h"
#include "eval.h"
#include "eval_detail.h"
#include "rt_val.h"

static void para_error_invalid_argc(char *func, int count)
{
    err_push("EVAL",
        "Incorrect arguments count passed to _%s_: %d",
        func, count);
}

static void para_error_arg_expected(char *func, int index, char *expected)
{
    err_push("EVAL",
            "Argument %d of parafunc _%s_ must be %s",
            index, func, expected);
}

static void eval_parafunc_if(
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct AstNode *args)
{
    VAL_LOC_T test_loc, temp_begin, temp_end;
    VAL_BOOL_T test_val;
    int argc = ast_list_len(args);

    if (argc != 3) {
        para_error_invalid_argc("if", argc);
        return;
    }

    temp_begin = rt->stack.top;
    test_loc = eval_impl(args, rt, sym_map);
    temp_end = rt->stack.top;

    if (err_state()) {
        err_push_src("EVAL", args->loc, "Failed evaluating if test");
        return;
    }

    if (rt_val_peek_type(&rt->stack, test_loc) != VAL_BOOL) {
        para_error_arg_expected("if", 1, "boolean");
        stack_collapse(&rt->stack, temp_begin, temp_end);
        return;
    }

    test_val = rt_val_peek_bool(rt, test_loc);
    stack_collapse(&rt->stack, temp_begin, temp_end);

    if (test_val) {
        eval_impl(args->next, rt, sym_map);
    } else {
        eval_impl(args->next->next, rt, sym_map);
    }
}

static void eval_parafunc_while(
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct AstNode *args)
{
    int argc = ast_list_len(args);
    bool done = false;

    if (argc != 2) {
        para_error_invalid_argc("while", argc);
        return;
    }

    while (!done) {

        VAL_LOC_T test_loc, temp_begin, temp_end;
        VAL_BOOL_T test_val;

        temp_begin = rt->stack.top;
        test_loc = eval_impl(args, rt, sym_map);
        temp_end = rt->stack.top;

        if (err_state()) {
            err_push_src("EVAL", args->loc, "Failed evaluating while test");
            return;
        }

        if (rt_val_peek_type(&rt->stack, test_loc) != VAL_BOOL) {
            para_error_arg_expected("while", 1, "boolean");
            stack_collapse(&rt->stack, temp_begin, temp_end);
            return;
        }

        test_val = rt_val_peek_bool(rt, test_loc);
        if (test_val) {
            eval_impl(args->next, rt, sym_map);
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

static void eval_parafunc_logic(
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

        VAL_LOC_T loc = eval_impl(args, rt, sym_map);
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

static void eval_parafunc_ref(
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct AstNode *args)
{
    int len = ast_list_len(args);
    char *symbol;
    struct SymMapNode *smn;

    if (len != 1) {
        para_error_invalid_argc("ref", len);
        return;
    }

    if (args->type != AST_REFERENCE) {
        para_error_arg_expected("ref", 1, "symbol");
        return;
    }
    symbol = args->data.reference.symbol;
    smn = sym_map_find(sym_map, symbol);

    if (!smn) {
        eval_error_not_found(symbol);
        return;
    }

    rt_val_push_ref(&rt->stack, smn->stack_loc);
}

static void eval_parafunc_peek(
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct AstNode *args)
{
    int len;
    VAL_LOC_T ref_loc, target_loc;
    VAL_LOC_T temp_begin, temp_end;
    enum ValueType ref_type;

    len = ast_list_len(args);
    if (len != 1) {
        para_error_invalid_argc("peek", len);
        return;
    }

    temp_begin = rt->stack.top;
    ref_loc = eval_impl(args, rt, sym_map);
    temp_end = rt->stack.top;
    if (err_state()) {
        err_push_src("EVAL", args->loc, "Failed evaluating _peek_ reference argument");
        return;
    }

    ref_type = rt_val_peek_type(&rt->stack, ref_loc);
    if (ref_type != VAL_REF) {
        para_error_arg_expected("peek", 1, "reference");
        return;
    }

    target_loc = rt_val_peek_ref(rt, ref_loc);
    rt_val_push_copy(&rt->stack, target_loc);
    stack_collapse(&rt->stack, temp_begin, temp_end);
}

static void eval_parafunc_poke(
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct AstNode *args)
{
    int len;
    VAL_LOC_T ref_loc, source_loc, target_loc;
    VAL_LOC_T temp_begin, temp_end;
    enum ValueType ref_type;

    len = ast_list_len(args);
    if (len != 2) {
        para_error_invalid_argc("poke", len);
        return;
    }

    temp_begin = rt->stack.top;
    ref_loc = eval_impl(args, rt, sym_map);
    if (err_state()) {
        err_push_src("EVAL", args->loc, "Failed evaluating _poke_ reference argument");
        return;
    }

    ref_type = rt_val_peek_type(&rt->stack, ref_loc);
    if (ref_type != VAL_REF) {
        para_error_arg_expected("poke", 1, "reference");
        return;
    }

    target_loc = rt_val_peek_ref(rt, ref_loc);

    source_loc = eval_impl(args->next, rt, sym_map);
    temp_end = rt->stack.top;
    if (err_state()) {
        err_push_src("EVAL", args->loc, "Failed evaluating _poke_ source argument");
        return;
    }

    if (!rt_val_pair_homo(rt, source_loc, target_loc)) {
        err_push_src("EVAL", args->loc, "Attempted to _poke_ value of mismatched type");
        return;
    }

    rt_val_poke_copy(&rt->stack, target_loc, source_loc);
    stack_collapse(&rt->stack, temp_begin, temp_end);
}

static void eval_parafunc_begin(
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct AstNode *args)
{
    int len = ast_list_len(args);
    char *symbol;
    struct SymMapNode *smn;
    VAL_LOC_T cpd_loc;
    enum ValueType ref_type;

    if (len != 1) {
        para_error_invalid_argc("begin", len);
        return;
    }

    if (args->type != AST_REFERENCE) {
        para_error_arg_expected("begin", 1, "symbol");
        return;
    }
    symbol = args->data.reference.symbol;
    smn = sym_map_find(sym_map, symbol);

    if (!smn) {
        eval_error_not_found(symbol);
        return;
    }

    cpd_loc = smn->stack_loc;
    ref_type = rt_val_peek_type(&rt->stack, cpd_loc);
    if (ref_type != VAL_ARRAY && ref_type != VAL_TUPLE) {
        para_error_arg_expected("begin", 1, "reference to compound object");
        return;
    }

    rt_val_push_ref(&rt->stack, rt_val_cpd_first_loc(cpd_loc));
}

static void eval_parafunc_end(
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct AstNode *args)
{
    int i, cpd_len, len = ast_list_len(args);
    char *symbol;
    struct SymMapNode *smn;
    VAL_LOC_T cpd_loc;
    enum ValueType ref_type;

    if (len != 1) {
        para_error_invalid_argc("end", len);
        return;
    }

    if (args->type != AST_REFERENCE) {
        para_error_arg_expected("end", 1, "symbol");
        return;
    }
    symbol = args->data.reference.symbol;
    smn = sym_map_find(sym_map, symbol);

    if (!smn) {
        eval_error_not_found(symbol);
        return;
    }

    cpd_loc = smn->stack_loc;
    ref_type = rt_val_peek_type(&rt->stack, cpd_loc);
    if (ref_type != VAL_ARRAY && ref_type != VAL_TUPLE) {
        para_error_arg_expected("end", 1, "reference to compound object");
        return;
    }

    cpd_len = rt_val_cpd_len(rt, cpd_loc);
    cpd_loc = rt_val_cpd_first_loc(cpd_loc);
    for (i = 0; i < cpd_len; ++i) {
        cpd_loc = rt_val_next_loc(rt, cpd_loc);
    }

    rt_val_push_ref(&rt->stack, cpd_loc);
}

static void eval_parafunc_inc(
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct AstNode *args)
{
    int len;
    struct SymMapNode *smn;
    VAL_LOC_T ref_loc, temp_loc;
    enum ValueType ref_type;

    len = ast_list_len(args);
    if (len != 1) {
        para_error_invalid_argc("inc", len);
    }

    if (args->type != AST_REFERENCE) {
        para_error_arg_expected("inc", 1, "plain reference");
        return;
    }

    smn = sym_map_find(sym_map, args->data.reference.symbol);
    if (!smn) {
        eval_error_not_found(args->data.reference.symbol);
        return;
    }
    ref_loc = smn->stack_loc;

    ref_type = rt_val_peek_type(&rt->stack, ref_loc);
    if (ref_type != VAL_REF) {
        para_error_arg_expected("inc", 1, "reference to reference");
        return;
    }

    temp_loc = rt_val_peek_ref(rt, ref_loc);
    temp_loc = rt_val_next_loc(rt, temp_loc);
    rt_val_poke_ref(&rt->stack, ref_loc, temp_loc);

    rt_val_push_unit(&rt->stack);
}

static void eval_parafunc_succ(
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct AstNode *args)
{
    int len;
    VAL_LOC_T temp_begin, temp_end;
    VAL_LOC_T ref_loc, target_loc;
    enum ValueType ref_type;

    len = ast_list_len(args);
    if (len != 1) {
        para_error_invalid_argc("succ", len);
        return;
    }

    temp_begin = rt->stack.top;
    ref_loc = eval_impl(args, rt, sym_map);
    temp_end = rt->stack.top;
    if (err_state()) {
        err_push_src("EVAL", args->loc, "Failed evaluating _succ_ reference argument");
        return;
    }

    ref_type = rt_val_peek_type(&rt->stack, ref_loc);
    if (ref_type != VAL_REF) {
        para_error_arg_expected("succ", 1, "reference");
        return;
    }

    target_loc = rt_val_peek_ref(rt, ref_loc);
    target_loc = rt_val_next_loc(rt, target_loc);
    rt_val_push_ref(&rt->stack, target_loc);
    stack_collapse(&rt->stack, temp_begin, temp_end);
}

void eval_parafunc(
        struct AstNode *node,
        struct Runtime *rt,
        struct SymMap *sym_map)
{
    enum AstParafuncType type = node->data.parafunc.type;
    struct AstNode *args = node->data.parafunc.args;

    switch (type) {
    case AST_PARAFUNC_IF:
        eval_parafunc_if(rt, sym_map, args);
        break;

    case AST_PARAFUNC_WHILE:
        eval_parafunc_while(rt, sym_map, args);
        break;

    case AST_PARAFUNC_AND:
        eval_parafunc_logic(rt, sym_map, args, false, "and");
        break;

    case AST_PARAFUNC_OR:
        eval_parafunc_logic(rt, sym_map, args, true, "or");
        break;

    case AST_PARAFUNC_REF:
        eval_parafunc_ref(rt, sym_map, args);
        break;

    case AST_PARAFUNC_PEEK:
        eval_parafunc_peek(rt, sym_map, args);
        break;

    case AST_PARAFUNC_POKE:
        eval_parafunc_poke(rt, sym_map, args);
        break;

    case AST_PARAFUNC_BEGIN:
        eval_parafunc_begin(rt, sym_map, args);
        break;

    case AST_PARAFUNC_END:
        eval_parafunc_end(rt, sym_map, args);
        break;

    case AST_PARAFUNC_INC:
        eval_parafunc_inc(rt, sym_map, args);
        break;

    case AST_PARAFUNC_SUCC:
        eval_parafunc_succ(rt, sym_map, args);
        break;
    }
}
