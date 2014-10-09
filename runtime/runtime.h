/* Copyright (C) 2014 Krzysztof Stachowiak */

#ifndef RUNTIME_H
#define RUNTIME_H

#include <stdbool.h>

#include "ast.h"
#include "stack.h"
#include "symmap.h"

struct Runtime {
    struct Stack *stack;
    struct SymMap global_sym_map;
    struct AstNode *node_store;
    VAL_LOC_T saved_loc;
    struct AstNode *saved_store;
};

struct Runtime *rt_make(long stack);
void rt_reset(struct Runtime *rt);
void rt_free(struct Runtime *rt);

void rt_save(struct Runtime *rt);
void rt_restore(struct Runtime *rt);

VAL_LOC_T rt_current_top(struct Runtime *rt);

void rt_consume_one(
        struct Runtime *rt,
        struct AstNode *ast,
        VAL_LOC_T *loc,
        struct AstNode **next);

bool rt_consume_list(struct Runtime *rt, struct AstNode *ast);

void rt_for_each_stack_val(struct Runtime *rt, void(*f)(void*, VAL_LOC_T));
void rt_for_each_sym(struct Runtime *rt, void(*f)(void*, char*, VAL_LOC_T));

void rt_val_print(struct Runtime *rt, VAL_LOC_T loc, bool annotate);
VAL_LOC_T rt_next_loc(struct Runtime *rt, VAL_LOC_T loc);

enum ValueType rt_val_type(struct Runtime *rt, VAL_LOC_T loc);
VAL_SIZE_T rt_val_size(struct Runtime *rt, VAL_LOC_T loc);

VAL_SIZE_T rt_peek_size(struct Runtime *rt, VAL_LOC_T loc);
void *rt_peek_ptr(struct Runtime *rt, VAL_LOC_T loc);

VAL_BOOL_T rt_peek_val_bool(struct Runtime *rt, VAL_LOC_T loc);
VAL_CHAR_T rt_peek_val_char(struct Runtime *rt, VAL_LOC_T loc);
VAL_INT_T rt_peek_val_int(struct Runtime *rt, VAL_LOC_T loc);
VAL_REAL_T rt_peek_val_real(struct Runtime *rt, VAL_LOC_T loc);
char* rt_peek_val_string(struct Runtime *rt, VAL_LOC_T loc);

VAL_LOC_T rt_peek_val_cpd_first(struct Runtime *rt, VAL_LOC_T loc);

void rt_peek_val_fun_locs(
        struct Runtime *rt, 
        VAL_LOC_T loc,
        VAL_LOC_T *impl_loc,
        VAL_LOC_T *cap_start,
        VAL_LOC_T *appl_start);

char *rt_fun_cap_symbol(struct Runtime *rt, VAL_LOC_T cap_loc);
VAL_LOC_T rt_fun_cap_val_loc(struct Runtime *rt, VAL_LOC_T cap_loc);

VAL_LOC_T rt_fun_next_cap_loc(struct Runtime *rt, VAL_LOC_T loc);
VAL_LOC_T rt_fun_next_appl_loc(struct Runtime *rt, VAL_LOC_T loc);

#endif
