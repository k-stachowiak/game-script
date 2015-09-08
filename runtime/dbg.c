/* Copyright (C) 2015 Krzysztof Stachowiak */

#include <stdio.h>

#include "collection.h"
#include "rt_val.h"
#include "timer_stack.h"
#include "dbg.h"

static void dbg_print_node_bind(struct AstBind *bind)
{
    if (bind->pattern->type == PATTERN_SYMBOL) {
        printf("bind \"%s\"\n", bind->pattern->data.symbol.symbol);
    }
}

static void dbg_print_node_compound(struct AstCompound *compound)
{
    printf("%s\n", (compound->type == AST_CPD_ARRAY) ? "array" : "tuple");
}

static void dbg_print_node_parafunc(struct AstParafunc *parafunc)
{
    switch (parafunc->type) {
    case AST_PARAFUNC_AND:
        printf("(and ...)\n");
        break;
    case AST_PARAFUNC_OR:
        printf("(or ...)\n");
        break;
    case AST_PARAFUNC_IF:
        printf("(if ...)\n");
        break;
    case AST_PARAFUNC_WHILE:
        printf("(while ...)\n");
        break;
    case AST_PARAFUNC_SWITCH:
        printf("(switch ...)\n");
        break;
    case AST_PARAFUNC_REF:
        printf("(ref ...)\n");
        break;
    case AST_PARAFUNC_PEEK:
        printf("(peek ...)\n");
        break;
    case AST_PARAFUNC_POKE:
        printf("(poke ...)\n");
        break;
    case AST_PARAFUNC_BEGIN:
        printf("(begin ...)\n");
        break;
    case AST_PARAFUNC_END:
        printf("(end ...)\n");
        break;
    case AST_PARAFUNC_INC:
        printf("(inc ...)\n");
        break;
    case AST_PARAFUNC_SUCC:
        printf("(succ ...)\n");
        break;
    }
}

static void dbg_print_node_func_call(struct AstFuncCall *func_call)
{
    char *symbol;

    if (func_call->func->type == AST_REFERENCE) {
        symbol = func_call->func->data.reference.symbol;
    } else {
        symbol = "<function expression>";
    }

    printf("%s()\n", symbol);
}

static void dbg_print_node_literal(struct AstLiteral *literal)
{
    switch (literal->type) {
    case AST_LIT_UNIT:
        printf("unit\n");
        break;
    case AST_LIT_BOOL:
        printf("%s\n", literal->data.boolean ? "true" : "false");
        break;
    case AST_LIT_STRING:
        printf("%s\n", literal->data.string);
        break;
    case AST_LIT_CHAR:
        printf("'%c'\n", literal->data.character);
        break;
    case AST_LIT_INT:
        printf("%ld\n", literal->data.integer);
        break;
    case AST_LIT_REAL:
        printf("%f\n", literal->data.real);
        break;
    }
}

static void dbg_print_node_reference(struct AstReference *reference)
{
    printf("ref %s\n", reference->symbol);
}

static void dbg_print_node(struct AstNode *node)
{
    switch (node->type) {
    case AST_DO_BLOCK:
        printf("do\n");
        break;

    case AST_BIND:
        dbg_print_node_bind(&node->data.bind);
        break;

    case AST_COMPOUND:
        dbg_print_node_compound(&node->data.compound);
        break;

    case AST_PARAFUNC:
        dbg_print_node_parafunc(&node->data.parafunc);
        break;

    case AST_FUNC_CALL:
        dbg_print_node_func_call(&node->data.func_call);
        break;

    case AST_FUNC_DEF:
        printf("function definition\n");
        break;

    case AST_LITERAL:
        printf("literal ");
        dbg_print_node_literal(&node->data.literal);
        break;

    case AST_REFERENCE:
        dbg_print_node_reference(&node->data.reference);
        break;
    }
}

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
    dbg_print_indent(dbg);
    dbg_print_node(node);
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

