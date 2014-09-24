/* Copyright (C) 2014 Krzysztof Stachowiak */

#include <stdio.h>
#include <stdbool.h>

#include "test.h"
#include "dom.h"
#include "lex.h"
#include "parse.h"
#include "runtime.h"

static bool lexer_test()
{
	struct DomNode *dom_node;

	if ((dom_node = lex("\"undelimited atom")) != NULL) {
		printf("Failed recognizing undelimited atom.\n");
		return false;
	}

	if ((dom_node = lex("( atom ( )")) != NULL) {
		printf("Failed recognizing undelimited compound DOM node.\n");
		return false;
	}

	if ((dom_node = lex("this (should () be (lexed) nicely)")) == NULL) {
		printf("Failed lexing a valid symbolic expression document.\n");
		return false;
	}

	dom_free(dom_node); /* Let valgrind keep an eye on this. */
	return true;
}

static bool parser_test()
{
	char *stupid_source =
		"(bind first (x) (car x))\n\n"
		"(bind stupid (func () (do\n"
		"\t(bind stupid1 { true \"super\" '\n' 1 2.0 })\n"
		"\t(bind first-of (first [true false true false]))\n"
		"\t(if first-of 'a' 'b')\n"
		")))";

	struct DomNode *dom_node;
	struct AstNode *ast_node;

	if (!(dom_node = lex(stupid_source))) {
		printf("Failed lexing non-trivial stupid source sample.\n");
		return false;
	}

	if (!(ast_node = parse(dom_node))) {
		printf("Failed parsing non-trivial stupid source sample.\n");
		dom_free(dom_node);
		return false;
	}

	ast_node_free(ast_node);
	dom_free(dom_node);

	return true;
}

int test(void)
{
	int result = 0;

	rt_init();
	if (!lexer_test() ||
		!parser_test()) {
		result = 1;
	}

	rt_deinit();

	if (result == 0) {
		printf("Tests ran successfully.\n");
	}

	return result;
}
