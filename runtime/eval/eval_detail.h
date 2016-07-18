/* Copyright (C) 2014,2015 Krzysztof Stachowiak */

#include "ast.h"
#include "runtime.h"

void eval_error_not_found(char *symbol);

void eval_special(
        struct AstSpecial *special,
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct SourceLocation *src_loc);

void eval_special_func_def(
        struct AstSpecFuncDef* func_def,
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct SourceLocation *src_loc);

void eval_special_bind_pattern(
        struct Pattern *pattern,
        VAL_LOC_T location,
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct SourceLocation *src_loc);

void eval_func_call(
        struct AstFuncCall *fcall,
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct SourceLocation *src_loc);

void eval_literal_compound(
        struct AstLiteralCompound *literal_compound,
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct SourceLocation *src_loc);

void eval_literal_atomic(
        struct AstLiteralAtomic *literal_atomic,
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct SourceLocation *src_loc);

VAL_LOC_T eval_dispatch(struct AstNode *node, struct Runtime *rt, struct SymMap *sym_map);
