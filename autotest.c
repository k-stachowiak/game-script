/* Copyright (C) 2014 Krzysztof Stachowiak */

#include <stddef.h>
#include <stdio.h>

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

static char *autotest_load_file(char *filename)
{
	char *buffer = 0;
	long length;
	FILE *file = fopen(filename, "rb");

	if (file) {
		fseek(file, 0, SEEK_END);
		length = ftell(file);
		fseek(file, 0, SEEK_SET);
		buffer = calloc(length + 1, 1);
		if (buffer) {
			fread(buffer, 1, length, file);
		}
		fclose(file);
	}

	return buffer;
}

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

static int autotest_load_module(char *source)
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

static int autotest_perform(void)
{
	struct AstNode *ast;
	ptrdiff_t location;
	struct Value val;

	ast = autotest_parse("(min-element [3 1 2])");

	if ((location = eval(ast, stack, &sym_map)) == -1) {
		return 1;
	}

	val = stack_peek_value(stack, location);
	val_print(&val, true);

	return 0;
}

int autotest(void)
{
    int error;
	char *source;

    stack = stack_make(1024);
    sym_map_init(&sym_map, NULL, stack);

	source = autotest_load_file("min-element.mn");
	error = autotest_load_module(source);

	if (error == 0) {
		error = autotest_perform();
	}

    sym_map_deinit(&sym_map);
    stack_free(stack);

    return error;
}
