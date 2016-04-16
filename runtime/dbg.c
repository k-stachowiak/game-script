/* Copyright (C) 2015 Krzysztof Stachowiak */

#include <stdio.h>

#include "collection.h"
#include "rt_val.h"
#include "timer_stack.h"
#include "dbg.h"

static void dbg_print_node_bind(struct AstCtlBind *bind)
{
    if (bind->pattern->type == PATTERN_SYMBOL) {
        printf("bind \"%s\"\n", bind->pattern->data.symbol.symbol);
    }
}

static void dbg_print_node_compound(struct AstCompound *compound)
{
    printf("%s\n", (compound->type == AST_CPD_ARRAY) ? "array" : "tuple");
}

static void dbg_print_node_special(struct AstSpecial *special)
{
    switch (special->type) {
    case AST_SPECIAL_AND:
        printf("(and ...)\n");
        break;
    case AST_SPECIAL_OR:
        printf("(or ...)\n");
        break;
    case AST_SPECIAL_IF:
        printf("(if ...)\n");
        break;
    case AST_SPECIAL_WHILE:
        printf("(while ...)\n");
        break;
    case AST_SPECIAL_REF:
        printf("(ref ...)\n");
        break;
    case AST_SPECIAL_PEEK:
        printf("(peek ...)\n");
        break;
    case AST_SPECIAL_POKE:
        printf("(poke ...)\n");
        break;
    case AST_SPECIAL_BEGIN:
        printf("(begin ...)\n");
        break;
    case AST_SPECIAL_END:
        printf("(end ...)\n");
        break;
    case AST_SPECIAL_INC:
        printf("(inc ...)\n");
        break;
    case AST_SPECIAL_SUCC:
        printf("(succ ...)\n");
        break;
    }
}

static void dbg_print_node_func_call(struct AstCtlFuncCall *func_call)
{
    char *symbol;

    if (func_call->func->type == AST_CONTROL &&
        func_call->func->data.control.type == AST_CTL_REFERENCE) {
            symbol = func_call->func->data.control.data.reference.symbol;
    } else {
        symbol = "<function expression>";
    }

    printf("%s()\n", symbol);
}

static void dbg_print_node_func_def(struct AstCtlFuncDef *func_def)
{
    (void)func_def;
    printf("function definition\n");
}

static void dbg_print_node_match(struct AstCtlMatch *match)
{
    (void)match;
    printf("match expression");
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

static void dbg_print_node_reference(struct AstCtlReference *reference)
{
    printf("ref %s\n", reference->symbol);
}

static void dbg_print_node(struct AstNode *node)
{
    switch (node->type) {
    case AST_CONTROL:
        switch (node->data.control.type) {
        case AST_CTL_DO:
            printf("do\n");
            break;

        case AST_CTL_BIND:
            dbg_print_node_bind(&node->data.control.data.bind);
            break;

        case AST_CTL_MATCH:
            dbg_print_node_match(&node->data.control.data.match);
            break;

        case AST_CTL_FUNC_DEF:
            dbg_print_node_func_def(&node->data.control.data.fdef);
            break;

        case AST_CTL_FUNC_CALL:
            dbg_print_node_func_call(&node->data.control.data.fcall);
            break;

        case AST_CTL_REFERENCE:
            dbg_print_node_reference(&node->data.control.data.reference);
            break;
        }
        break;

    case AST_COMPOUND:
        dbg_print_node_compound(&node->data.compound);
        break;

    case AST_SPECIAL:
        dbg_print_node_special(&node->data.special);
        break;

    case AST_LITERAL:
        printf("literal ");
        dbg_print_node_literal(&node->data.literal);
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

