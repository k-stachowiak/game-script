/* Copyright (C) 2014-2016 Krzysztof Stachowiak */

#include "ast.h"
#include "runtime.h"

void eval_error_not_found_src(char *symbol, struct SourceLocation *loc);

void eval_special(
    struct AstNode *node,
    struct Runtime *rt,
    struct SymMap *sym_map,
    struct AstLocMap *alm);

void eval_special_func_def(
    struct AstNode* node,
    struct Runtime *rt,
    struct SymMap *sym_map,
    struct AstLocMap *alm);

void eval_special_bind_pattern(
    struct AstNode *pattern,
    VAL_LOC_T location,
    struct Runtime *rt,
    struct SymMap *sym_map,
    struct AstLocMap *alm);

void eval_special_set_of(
    struct AstNode *node,
    struct Runtime *rt,
    struct SymMap *sym_map,
    struct AstLocMap *alm);

void eval_special_range_of(
    struct AstNode *node,
    struct Runtime *rt,
    struct SymMap *sym_map,
    struct AstLocMap *alm);

void eval_special_array_of(
    struct AstNode *node,
    struct Runtime *rt,
    struct SymMap *sym_map,
    struct AstLocMap *alm);

void eval_special_tuple_of(
    struct AstNode *node,
    struct Runtime *rt,
    struct SymMap *sym_map,
    struct AstLocMap *alm);

void eval_special_pointer_to(
    struct AstNode *node,
    struct Runtime *rt,
    struct SymMap *sym_map,
    struct AstLocMap *alm);

void eval_special_function_type(
    struct AstNode *node,
    struct Runtime *rt,
    struct SymMap *sym_map,
    struct AstLocMap *alm);

void eval_special_type_op(
    struct AstNode *node,
    struct Runtime *rt,
    struct SymMap *sym_map,
    struct AstLocMap *alm,
    bool is_product);

void eval_func_call(
    struct AstNode *node,
    struct Runtime *rt,
    struct SymMap *sym_map,
    struct AstLocMap *alm);

void eval_literal_compound(
    struct AstNode *node,
    struct Runtime *rt,
    struct SymMap *sym_map,
    struct AstLocMap *alm);

void eval_literal_atomic(
    struct AstNode *node,
    struct Runtime *rt,
    struct SymMap *sym_map,
    struct AstLocMap *alm);

VAL_LOC_T eval_dispatch(
    struct AstNode *node,
    struct Runtime *rt,
    struct SymMap *sym_map,
    struct AstLocMap *alm);
