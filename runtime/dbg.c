/* Copyright (C) 2014-2016 Krzysztof Stachowiak */

#include <stdio.h>

#include "collection.h"
#include "rt_val.h"
#include "timer_stack.h"
#include "dbg.h"

static void dbg_print_indent(struct Debugger *dbg)
{
    static const int indent = 4;
    int i;
    for (i = 0; i < dbg->lvl * indent; ++i) {
        if (!(i % indent)) {
            putc('.', stdout);
        } else {
            putc(' ', stdout);
        }
    }
}

void dbg_init(struct Debugger *dbg)
{
    dbg->lvl = 0;
}

void dbg_deinit(struct Debugger *dbg)
{
    (void)dbg;
}

void dbg_call_begin(void *dbg_void, struct AstNode* node)
{
    struct Debugger *dbg = (struct Debugger*)dbg_void;
    char *string = ast_serialize(node);
    dbg_print_indent(dbg);
    printf("%s\n", string);
    mem_free(string);
    ++dbg->lvl;
}

void dbg_call_end(
        void *dbg_void,
        struct Runtime* rt,
        VAL_LOC_T val_loc,
        bool error)
{
    struct Debugger *dbg = (struct Debugger*)dbg_void;

    if (error) {
        dbg->lvl = 0;
    } else {
        --dbg->lvl;
        dbg_print_indent(dbg);
        printf("`~~~~~> ");
        rt_val_print(rt, val_loc, false);
        printf("\n");

    }
}

