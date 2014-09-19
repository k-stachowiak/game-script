/* Copyright (C) 2014 Krzysztof Stachowiak */

#include <stdlib.h>

#include "ast.h"
#include "ast_parse.h"
#include "value.h"
#include "error.h"
#include "stack.h"
#include "eval.h"
#include "common.h"

#include "repl_expr.h"
#include "runtime.h"

enum ReplExprResult repl_expr_command(char *expression_line)
{
	struct AstNode *ast;
	VAL_LOC_T location;
	struct Value val;

	ast = ast_parse_source(expression_line);
	if (!ast) {
		printf("Error: %s\n", err_msg());
		err_reset();
		return REPL_EXPR_ERROR;
	}
		
	location = rt_consume_one(ast);
	if (err_state()) {
		printf("Error : %s\n", err_msg());
		return REPL_EXPR_ERROR;
	}

	val = rt_peek(location);
	val_print(&val, true);

	printf("\n");		

	return REPL_EXPR_OK;
}
