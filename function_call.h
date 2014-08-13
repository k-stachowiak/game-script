/* Copyright (C) 2014 Krzysztof Stachowiak */

#ifndef FUNCTION_CALL_H
#define FUNCTION_CALL_H

#include "itpr.h"
#include "ast.h"

void call_function(
	struct Stack *stack,
	struct SymMap *sym_map,
	struct Value val,
	struct AstNode *call_node);

#endif
