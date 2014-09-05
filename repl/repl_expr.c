/* Copyright (C) 2014 Krzysztof Stachowiak */

#include <stdlib.h>

#include "ast.h"
#include "lex.h"
#include "parse.h"
#include "value.h"
#include "error.h"
#include "stack.h"
#include "eval.h"
#include "common.h"

#include "repl_expr.h"
#include "repl_state.h"

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

enum ReplExprResult repl_expr_command(char *expression_line)
{
	struct AstNode *ast;
	VAL_LOC_T location;
	struct Value val;

	ast = repl_parse(expression_line);
	if (!ast) {
		printf("Error: %s\n", err_msg());
		err_reset();
		return REPL_EXPR_ERROR;
	}
		
	location = repl_state_consume(ast);
	if (err_state()) {
		printf("Error : %s\n", err_msg());
		return REPL_EXPR_ERROR;
	}

	val = repl_state_peek(location);
	val_print(&val, true);

	printf("\n");		

	return REPL_EXPR_OK;
}