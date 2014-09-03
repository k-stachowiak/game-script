/* Copyright (C) 2014 Krzysztof Stachowiak */

#ifndef REPL_EXPR_H
#define REPL_EXPR_H

enum ReplExprResult {
	REPL_EXPR_OK,
	REPL_EXPR_ERROR
};

enum ReplExprResult repl_expr_command(char *expression_line);

#endif