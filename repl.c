/* Copyright (C) 2014 Krzysztof Stachowiak */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "common.h"
#include "lex.h"
#include "parse.h"
#include "stack.h"
#include "value.h"
#include "eval.h"
#include "error.h"

static bool eof_flag;
static struct Stack *stack;
static struct SymMap sym_map;

static bool request_quit = false;

static struct {
    struct AstNode **data;
    int size, cap;
} stored_nodes;

/*
 * Implementation copied from StackOverflow :
 * http://stackoverflow.com/questions/314401/how-to-read-a-line-from-the-console-in-c
 * It has been slightly modified to fit into the program, e.g. error handling.
 */
static char *repl_getline(void)
{
    char *line = malloc(100);
    char *linep = line;
    size_t lenmax = 100;
    size_t len = lenmax;
    int c;

    if (line == NULL) {
        err_set(ERR_REPL, "Allocation failed.");
        return NULL;
    }

    for (;;) {
        c = fgetc(stdin);
        if(c == EOF) {
            eof_flag = true;
            break;
        }

        if (--len == 0) {
            len = lenmax;
            char *linen = realloc(linep, lenmax *= 2);

            if(linen == NULL) {
                free(linep);
                err_set(ERR_REPL, "Allocation failed.");
                return NULL;
            }

            line = linen + (line - linep);
            linep = linen;
        }

        if ((*line++ = c) == '\n') {
            break;
        }
    }

    *line = '\0';
    return linep;
}

static struct AstNode *repl_parse(char *line)
{
    struct DomNode *dom;
    struct AstNode *ast;

    if (!(dom = lex(line))) {
        return NULL;
    }

    if (!(ast = parse(dom))) {
        dom_free(dom);
        return NULL;
    }

    dom_free(dom);

    return ast;
}

static void repl_handle_command_load(char *pieces[], int num_pieces)
{

}

static void repl_handle_command(char *cmd)
{
	char *pieces[2];
	int num_pieces = 0;
	char *current = cmd;
	char *last = cmd + strlen(cmd);

	while (current != last) {
		pieces[num_pieces++] = current;
		while (*current && !isspace(*(current++)));
		*(current - 1) = '\0';
	}

	printf("\"%s\"", pieces[0]);

	if (strcmp(pieces[0], "ld") == 0) {
		repl_handle_command_load(pieces + 1, num_pieces - 1);
	} else if (strcmp(pieces[0], "q") == 0) {
		request_quit = true;
	} else {
		printf("Bad command or file name.\n");
	}
}

static void repl_handle_expression(char *expr)
{
	struct AstNode *ast;
	ptrdiff_t location;
	struct Value val;

	if (!(ast = repl_parse(expr))) {
		printf("Error: %s\n", err_msg());
		err_reset();

	} else {
		switch (ast->type) {
		case AST_LITERAL:
		case AST_COMPOUND:
		case AST_REFERENCE:
		case AST_FUNC_DEF:
		case AST_FUNC_CALL:
			location = stack->top;
			eval(ast, stack, &sym_map);
			val = stack_peek_value(stack, location);
			val_print(&val, true);
			break;

		case AST_BIND:
			location = stack->top;
			eval(ast, stack, &sym_map);
			val = stack_peek_value(stack, location);
			printf("Bound ");
			val_print(&val, true);
			printf(" to symbol \"%s\"", ast->data.bind.symbol);
			break;

		default:
			printf("Printing not handled for this type of AST node.");
			break;

		}
		printf("\n");

		if (ast->type == AST_BIND) {
			ARRAY_APPEND(stored_nodes, ast);
		}
		else {
			ast_node_free(ast);
		}
	}
}

static void repl_handle_line(char *line)
{
    if (strcmp(line, "") == 0) {
        return;
	} else if (*line == ':') {
		repl_handle_command(line + 1);
	} else {
		repl_handle_expression(line);
	}
}

static void repl_free_bound(void)
{
    while (stored_nodes.size) {
        ast_node_free(stored_nodes.data[0]);
        ARRAY_REMOVE(stored_nodes, 0);
    }
}

int repl(void)
{
    LOG_TRACE_FUNC
    eof_flag = false;
    stack = stack_make(1024);
    sym_map_init(&sym_map, NULL, stack);
    err_reset();

    for (;;) {

        int result = 0;
        char *line = NULL;

        printf("moon> ");
        line = repl_getline();

        if (err_state()) {
            result = 1;

        } else if (eof_flag) {
            free(line);
            repl_free_bound();
            sym_map_deinit(&sym_map);
            stack_free(stack);
            printf("\n");
            return 0;

        } else {
            repl_handle_line(line);
        }

        free(line);
        if (result) {
            repl_free_bound();
            sym_map_deinit(&sym_map);
            stack_free(stack);
            return result;

		} else if (request_quit) {
			printf("Quit request captured.\n");
			return 0;
		}
    }

    exit(1);
}
