#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "common.h"
#include "lex.h"
#include "parse.h"
#include "itpr.h"
#include "error.h"

static bool eof_flag;
static struct Stack *stack;

/*
 * Implementation copied from StackOverflow :
 * http://stackoverflow.com/questions/314401/how-to-read-a-line-from-the-console-in-c
 * It has been slightly modified to fit into the program, e.g. error handling.
 */
static char *XENO_getline(void)
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

static struct AstNode *read(char *line)
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

static void handle_line(char *line)
{
    struct AstNode *ast;

    if (strcmp(line, "") == 0) {
        return;

    } else if (!(ast = read(line))) {
        printf("Error: %s\n", err_msg());
        err_reset();

    } else {
        if (ast->type == AST_LITERAL) {
            ptrdiff_t location = eval(ast, stack);
            struct Value val = stack_peek(stack, location);
            val_print(&val);
            printf("\n");
        } else if (ast->type == AST_COMPOUND) {
            
        } else {
            printf("Printing not handled for this type of AST node.\n");
        }
        ast_node_free(ast);
    }
}

int repl(void)
{
    LOG_TRACE_FUNC
    eof_flag = false;
    stack = stack_make(1024);
    err_reset();

    for (;;) {

        int result = 0;
        char *line = NULL;

        printf("moon> ");
        line = XENO_getline();

        if (err_state()) {
            result = 1;

        } else if (eof_flag) {
            free(line);
            stack_free(stack);
            printf("\n");
            return 0;

        } else {
            handle_line(line);
        }

        free(line);
        if (result) {
            stack_free(stack);
            return result;
        }
    }

    exit(1);
}
