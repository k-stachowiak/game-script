/* Copyright (C) 2014,2015 Krzysztof Stachowiak */

#include <stdlib.h>

#include "log.h"
#include "ast.h"
#include "parse.h"
#include "error.h"
#include "stack.h"
#include "eval.h"

#include "repl_expr.h"
#include "runtime.h"
#include "rt_val.h"

static void repl_expr_error_ast_len(int actual)
{
    struct ErrMessage msg;
    err_msg_init(&msg, "REPL");
    err_msg_append(&msg,
        "Parsed more than one AST node (%d) - only one allowed", actual);
    err_msg_set(&msg);
}

static void repl_expr_error_ast_illegal(char *reason)
{
    struct ErrMessage msg;
    err_msg_init(&msg, "REPL");
    err_msg_append(&msg, "Illegal expression: %s", reason);
    err_msg_set(&msg);
}

enum ReplExprResult repl_expr_command(struct Runtime *rt, char *expression_line)
{
	struct AstNode *ast;
    int ast_len;
	VAL_LOC_T location;

	ast = parse_source(expression_line);
	if (!ast) {
		return REPL_EXPR_INTERNAL_ERROR;
	}

    ast_len = ast_list_len(ast);
    if (ast_len != 1) {
        repl_expr_error_ast_len(ast_len);
        return REPL_EXPR_ERROR;

    } else if (ast->type == AST_FUNC_DEF) {
        repl_expr_error_ast_illegal(
            "Function definition cannot prsist without being bound");
        return REPL_EXPR_ERROR;
    }

	rt_consume_one(rt, ast, &location, NULL);
	if (err_state()) {
		return REPL_EXPR_INTERNAL_ERROR;
	}

	rt_val_print(rt, location, true);
	printf("\n");
	return REPL_EXPR_OK;
}

