/* Copyright (C) 2014 Krzysztof Stachowiak */

#ifndef FUNCTION_CALL_H
#define FUNCTION_CALL_H

#include "itpr.h"
#include "ast.h"

ptrdiff_t call_function(
	struct Stack *stack,
	struct SymMap *sym_map,
	struct Value val,
	struct AstNode *node);

#endif
