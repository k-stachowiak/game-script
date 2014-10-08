/* Copyright (C) 2014 Krzysztof Stachowiak */

#ifndef REPL_EXPR_H
#define REPL_EXPR_H

#include "runtime.h"

enum ReplExprResult {
	REPL_EXPR_OK,
	REPL_EXPR_ERROR,
	REPL_EXPR_INTERNAL_ERROR
};

enum ReplExprResult repl_expr_command(struct Runtime *rt, char *expression_line);

#endif
