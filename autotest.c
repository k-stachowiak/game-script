/* Copyright (C) 2014 Krzysztof Stachowiak */

#include <stddef.h>

#include "common.h"
#include "lex.h"
#include "parse.h"
#include "stack.h"
#include "value.h"
#include "eval.h"

static struct Stack *stack;
static struct SymMap sym_map;

static struct {
    struct AstNode **data;
    int size, cap;
} stored_nodes;

static struct AstNode *autotest_parse(char *source)
{
    struct DomNode *dom;
    struct AstNode *ast;

    if (!(dom = lex(source))) {
        return NULL;
    }

    if (!(ast = parse(dom))) {
        dom_free(dom);
        return NULL;
    }

    dom_free(dom);

    return ast;
}

static int autotest_perform_one(char *source)
{
    struct AstNode *ast = autotest_parse(source);
    struct Value val;
    ptrdiff_t location;

    if (!ast) {
        return 1;
    }

    location = stack->top;
    eval(ast, stack, &sym_map);
    val = stack_peek_value(stack, location);
    val_print(&val, true);
    printf("\n");

    if (ast->type == AST_BIND) {
        ARRAY_APPEND(stored_nodes, ast);
    } else {
        ast_node_free(ast);
    }

    return 0;
}

static int autotest_perform_all(void)
{
    return autotest_perform_one("(bind add1 (+ 1))") ||
           autotest_perform_one("(add1 2)") ||
           autotest_perform_one("(bind sqr (func (x) (* x x)))") ||
           autotest_perform_one("(sqr 2)");
}

int autotest(void)
{
    int error;

    stack = stack_make(1024);
    sym_map_init(&sym_map, NULL, stack);

    error = autotest_perform_all();

    while (stored_nodes.size) {
        ast_node_free(stored_nodes.data[0]);
        ARRAY_REMOVE(stored_nodes, 0);
    }

    sym_map_deinit(&sym_map);
    stack_free(stack);

    return error;
}