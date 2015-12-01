/* Copyright (C) 2014,2015 Krzysztof Stachowiak */

#include "ast.h"
#include "runtime.h"

void eval_error_not_found(char *symbol);
void para_error_invalid_argc(char *func, int count);
void para_error_arg_expected(char *func, int index, char *expected);

void eval_control(
        struct AstControl *control,
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct SourceLocation *src_loc);

void eval_parafunc(struct AstParafunc *parafunc, struct Runtime *rt, struct SymMap *sym_map);
void eval_compound(struct AstCompound *compound, struct Runtime *rt, struct SymMap *sym_map);
void eval_literal(struct AstLiteral *literal, struct Runtime *rt, struct SymMap *sym_map);

void eval_func_call(struct AstCtlFuncCall *fcall, struct Runtime *rt, struct SymMap *sym_map);
void eval_func_def(struct AstCtlFuncDef *fdef, struct Runtime *rt, struct SymMap *sym_map);

void eval_parafunc_if(struct Runtime *rt, struct SymMap *sym_map, struct AstNode *args);
void eval_parafunc_while(struct Runtime *rt, struct SymMap *sym_map, struct AstNode *args);
void eval_parafunc_ref(struct Runtime *rt, struct SymMap *sym_map, struct AstNode *args);
void eval_parafunc_logic(struct Runtime *rt, struct SymMap *sym_map, struct AstNode *args, bool bv, char *f);
void eval_parafunc_ref(struct Runtime *rt, struct SymMap *sym_map, struct AstNode *args);
void eval_parafunc_peek(struct Runtime *rt, struct SymMap *sym_map, struct AstNode *args);
void eval_parafunc_poke(struct Runtime *rt, struct SymMap *sym_map, struct AstNode *args);
void eval_parafunc_begin(struct Runtime *rt, struct SymMap *sym_map, struct AstNode *args);
void eval_parafunc_end(struct Runtime *rt, struct SymMap *sym_map, struct AstNode *args);
void eval_parafunc_inc(struct Runtime *rt, struct SymMap *sym_map, struct AstNode *args);
void eval_parafunc_succ(struct Runtime *rt, struct SymMap *sym_map, struct AstNode *args);

void eval_bind_pattern(
        struct Pattern *pattern,
        struct Runtime *rt,
        struct SymMap *sym_map,
        VAL_LOC_T location,
        struct SourceLocation *source_loc);

VAL_LOC_T eval_dispatch(struct AstNode *node, struct Runtime *rt, struct SymMap *sym_map);
