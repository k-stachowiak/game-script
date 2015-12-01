/* Copyright (C) 2015 Krzysztof Stachowiak */

#include "runtime.h"
#include "symmap.h"
#include "error.h"
#include "ast.h"
#include "eval_detail.h"

void eval_parafunc_ref(
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

    if (args->type != AST_CONTROL || args->data.control.type != AST_CTL_REFERENCE) {
        para_error_arg_expected("ref", 1, "symbol");
        return;
    }
    symbol = args->data.control.data.reference.symbol;
    smn = sym_map_find(sym_map, symbol);

    if (!smn) {
        eval_error_not_found(symbol);
        return;
    }

    rt_val_push_ref(&rt->stack, smn->stack_loc);
}

void eval_parafunc_peek(
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
    ref_loc = eval_dispatch(args, rt, sym_map);
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

void eval_parafunc_poke(
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
    ref_loc = eval_dispatch(args, rt, sym_map);
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

    source_loc = eval_dispatch(args->next, rt, sym_map);
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

void eval_parafunc_begin(
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

    if (args->type != AST_CONTROL || args->data.control.type != AST_CTL_REFERENCE) {
        para_error_arg_expected("begin", 1, "symbol");
        return;
    }
    symbol = args->data.control.data.reference.symbol;
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

void eval_parafunc_end(
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

    if (args->type != AST_CONTROL || args->data.control.type != AST_CTL_REFERENCE) {
        para_error_arg_expected("end", 1, "symbol");
        return;
    }
    symbol = args->data.control.data.reference.symbol;
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

void eval_parafunc_inc(
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct AstNode *args)
{
    int len;
    char *symbol;
    struct SymMapNode *smn;
    VAL_LOC_T ref_loc, temp_loc;
    enum ValueType ref_type;

    len = ast_list_len(args);
    if (len != 1) {
        para_error_invalid_argc("inc", len);
    }

    if (args->type != AST_CONTROL || args->data.control.type != AST_CTL_REFERENCE) {
        para_error_arg_expected("inc", 1, "plain reference");
        return;
    }
    symbol = args->data.control.data.reference.symbol;

    smn = sym_map_find(sym_map, symbol);
    if (!smn) {
        eval_error_not_found(symbol);
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

void eval_parafunc_succ(
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
    ref_loc = eval_dispatch(args, rt, sym_map);
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

