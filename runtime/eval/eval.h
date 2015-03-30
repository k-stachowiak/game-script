/* Copyright (C) 2014 Krzysztof Stachowiak */

#ifndef EVAL_H
#define EVAL_H

#include "ast.h"
#include "stack.h"
#include "symmap.h"

#include <stddef.h>

#define RT_DEBUG false

#if RT_DEBUG
void efc_log_gen_call_sym(char *symbol);
void efc_log_gen_call_arg(struct Runtime *rt, VAL_LOC_T loc);
void efc_log_result(struct Runtime *rt, VAL_LOC_T loc);
void efc_log_bif_call(
        struct Runtime *rt,
        char *symbol,
        VAL_LOC_T arg_locs[],
        int arg_count,
        VAL_LOC_T result_loc);
#else
#    define efc_log_gen_call_sym(X)
#    define efc_log_gen_call_arg(X, Y)
#    define efc_log_result(X, Y)
#    define efc_log_bif_call(A, B, C, D, E)
#endif

void eval_init(void);

void eval_location_reset(void);
void eval_location_push(struct SourceLocation *loc);
void eval_location_swap(struct SourceLocation *loc);
void eval_location_pop(void);
struct SourceLocation *eval_location_top(void);

VAL_LOC_T eval(struct AstNode *node, struct Runtime *rt, struct SymMap *sym_map);
VAL_LOC_T eval_bif(struct Runtime *rt, void *impl, VAL_SIZE_T arity);

#endif
