/* Copyright (C) 2015 Krzysztof Stachowiak */

#ifndef DBG_H
#define DBG_H

#include "ast.h"
#include "rt_val.h"

struct Runtime;

struct Debugger {
    int lvl;
};

void dbg_init(struct Debugger *dbg);
void dbg_deinit(struct Debugger *dbg);
void dbg_call_begin(void *dbg_void, struct AstNode* node);
void dbg_call_end(
        void *dbg_void,
        struct Runtime* rt,
        VAL_LOC_T val_loc,
        bool error);

#endif
