/* Copyright (C) 2014 Krzysztof Stachowiak */

#ifndef EVAL_H
#define EVAL_H

#include "ast.h"
#include "stack.h"
#include "symmap.h"

#include <stddef.h>

void eval_location_reset(void);
void eval_location_push(struct SourceLocation loc);
void eval_location_swap(struct SourceLocation loc);
void eval_location_pop(void);
struct SourceLocation *eval_location_top(void);

ptrdiff_t eval(
    struct AstNode *node,
    struct Stack *stack,
    struct SymMap *sym_map);

#endif
