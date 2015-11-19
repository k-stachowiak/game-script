/* Copyright (C) 2014,2015 Krzysztof Stachowiak */

#include "ast.h"
#include "runtime.h"

void eval_error_not_found(char *symbol);

void eval_func_def(struct AstNode *node, struct Stack *stack, struct SymMap *sym_map);
void eval_func_call(struct AstNode *node, struct Runtime *rt, struct SymMap *sym_map);
void eval_compound(struct AstNode *node, struct Runtime *rt, struct SymMap *sym_map);
void eval_parafunc(struct AstNode *node, struct Runtime *rt, struct SymMap *sym_map);
void eval_bind(struct AstNode *node, struct Runtime *rt, struct SymMap *sym_map);
void eval_match(struct AstNode *node, struct Runtime *rt, struct SymMap *sym_map);

void eval_bind_pattern(
    struct Runtime *rt,
    struct SymMap *sym_map,
    struct Pattern *pattern,
    VAL_LOC_T location,
    struct SourceLocation *source_loc);

VAL_LOC_T eval_impl(struct AstNode *node, struct Runtime *rt, struct SymMap *sym_map);
