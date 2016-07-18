/* Copyright (C) 2014,2015 Krzysztof Stachowiak */

#ifndef RUNTIME_H
#define RUNTIME_H

#include <stdbool.h>

#include "dbg.h"
#include "ast.h"
#include "stack.h"
#include "symmap.h"
#include "moon.h"

struct Runtime {
    struct Stack stack;
    struct SymMap global_sym_map;
    struct Debugger debugger;

    struct AstNode *node_store;

    bool debug;

    VAL_LOC_T saved_loc;
    struct AstNode *saved_store;
};

struct Runtime *rt_make(void);
void rt_reset(struct Runtime *rt);
void rt_free(struct Runtime *rt);

void rt_save(struct Runtime *rt);
void rt_restore(struct Runtime *rt);

void rt_register_clif_handler(
        struct Runtime *rt,
        char *symbol,
        int arity,
        ClifHandler handler);

bool rt_consume_one(
        struct Runtime *rt,
        struct AstNode *ast,
        VAL_LOC_T *loc,
        struct AstNode **next);

bool rt_consume_list(
        struct Runtime *rt,
        struct AstNode *ast,
        VAL_LOC_T *last_loc);

#endif
