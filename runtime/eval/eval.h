/* Copyright (C) 2014 Krzysztof Stachowiak */

#ifndef EVAL_H
#define EVAL_H

#include "ast.h"
#include "stack.h"
#include "symmap.h"

#include <stddef.h>

VAL_LOC_T eval(struct AstNode *node, struct Runtime *rt, struct SymMap *sym_map);
VAL_LOC_T eval_bif(struct Runtime *rt, void *impl, VAL_SIZE_T arity);
VAL_LOC_T eval_clif(struct Runtime *rt, void *impl, VAL_SIZE_T arity);

#endif
