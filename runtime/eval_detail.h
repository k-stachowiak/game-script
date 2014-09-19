/* Copyright (C) 2014 Krzysztof Stachowiak */

#include "ast.h"
#include "stack.h"
#include "symmap.h"
#include "value.h"

void eval_error_not_found(char *symbol);

void eval_func_def(
	struct AstNode *node,
	struct Stack *stack,
	struct SymMap *sym_map);

void eval_func_call(
	struct AstNode *node,
	struct Stack *stack,
	struct SymMap *sym_map);

VAL_LOC_T eval_impl(
	struct AstNode *node,
	struct Stack *stack,
	struct SymMap *sym_map);
