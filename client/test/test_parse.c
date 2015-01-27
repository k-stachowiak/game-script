/* Copyright (C) 2015 Krzysztof Stachowiak */

#include <stdio.h>

#include "lex.h"
#include "parse.h"
#include "parse.h"

#include "test_detail.h"

bool test_lexer(void)
{
	struct DomNode *dom_node;

	if ((dom_node = lex("\"undelimited atom")) != NULL) {
		printf("Failed recognizing undelimited atom.\n");
		dom_free(dom_node);
		return false;
	}

	if ((dom_node = lex("( atom ( )")) != NULL) {
		printf("Failed recognizing undelimited compound DOM node.\n");
		dom_free(dom_node);
		return false;
	}

	if ((dom_node = lex("this (should () be (lexed) nicely)")) == NULL) {
		printf("Failed lexing a valid symbolic expression document.\n");
		return false;
	} else {
		dom_free(dom_node);
	}

	return true;
}

bool test_parser(void)
{
	int i;

	char *good_sources[] = {
		"a\nb",
		"(if true [][])",
		"(bind two (+ 1 1))",
		"(do (bind x \"ex\") (cat x x))",
		"(if true 1 2)",
		"[ 1 { 1.0 1.0 [ 'a' 'b' ] } ]",
		"(func (x) (* x x))",
		"{ true \"as\" '\\t' 6.0 1 x }",
	};

	int good_sources_count = sizeof(good_sources) / sizeof(good_sources[0]);

	for (i = 0; i < good_sources_count; ++i) {
		struct AstNode *ast_node = parse_source(good_sources[i]);
		if (!ast_node) {
            printf("Failed parsing source: \"%s\".\n", good_sources[i]);
			return false;
		}
		ast_node_free(ast_node);
	}

	return true;
}

