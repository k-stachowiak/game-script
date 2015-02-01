/* Copyright (C) 2014,2015 Krzysztof Stachowiak */

#include "ast.h"
#include "runtime.h"

void eval_error_not_found(char *symbol);

void eval_func_def(struct AstNode *node, struct Stack *stack, struct SymMap *sym_map);
void eval_func_call(struct AstNode *node, struct Runtime *rt, struct SymMap *sym_map);
void eval_compound(struct AstNode *node, struct Runtime *rt, struct SymMap *sym_map);
void eval_do_block(struct AstNode *node, struct Runtime *rt, struct SymMap *sym_map);
VAL_LOC_T eval_impl(struct AstNode *node, struct Runtime *rt, struct SymMap *sym_map);
