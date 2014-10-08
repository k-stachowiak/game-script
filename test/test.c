/* Copyright (C) 2014 Krzysztof Stachowiak */

#include <stdio.h>
#include <stdbool.h>

#include "error.h"
#include "test.h"
#include "dom.h"
#include "lex.h"
#include "parse.h"
#include "ast_parse.h"
#include "runtime.h"

static bool test_source_eval(struct Runtime *rt, char *source, VAL_LOC_T *locs)
{
	int i = 0;
	struct AstNode *ast_list = NULL, *next;

	err_reset();

	/* 1. Parse the input source. */
	ast_list = ast_parse_source(source);
	if (!ast_list) {
		printf("Error parsing source: %s.\n", source);
		printf("Error : %s.\n", err_msg());
		return false;
	}

	/* 2. Evaluate the source nodes. */
	while (ast_list) {
		rt_consume_one(rt, ast_list, locs + (i++), &next);
		if (err_state()) {
			printf("Failed consuming an AST node into runtime.\n");
			printf("Error : %s.\n", err_msg());
			ast_node_free(ast_list);
			return false;
		}
		ast_list = next;
	}

	return true;
}

static bool test_lexer()
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


static bool test_parser()
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
		struct AstNode *ast_node = ast_parse_source(good_sources[i]);
		if (!ast_node) {
			return false;
		}
		ast_node_free(ast_node);
	}

	printf("Parsed %d simple expressions successfully.\n", i);
	return true;
}

static bool test_runtime_sanity(struct Runtime *rt)
{
    VAL_LOC_T results[1];
	char *source = "(bind x \"x\")";
    return test_source_eval(rt, source, results);
}

static bool test_simple_algorithm(struct Runtime *rt)
{
	VAL_LOC_T results[4];
	char *source =
		"(bind gcd (func (x y) (if (= y 0) x (gcd y (% x y)) ) ))\n"
		"(bind lcm (func (x y) (* (/ x (gcd x y)) y ) ))\n"
		"(bind x (gcd 54 24)) # Expected to be 6\n"
		"(bind y (lcm 21 6)) # Expected to be 42";

	if (!test_source_eval(rt, source, results)) {
        return false;
	}

	if (rt_peek_val_int(rt, results[2]) != 6) {
		printf("gcd returned %ld, instead of 6.\n", rt_peek_val_int(rt, results[0]));
        return false;
	}

	if (rt_peek_val_int(rt, results[3]) != 42) {
		printf("lcm returned %ld, instead of 42.\n", rt_peek_val_int(rt, results[1]));
        return false;
	}

	return true;
}

static bool test_array_lookup(struct Runtime *rt)
{
	VAL_LOC_T results[3];
	char *source =
		"(bind min-element` (func (element array)\n"
		"    (if (empty array)\n"
		"		element\n"
        "        (if (< element (car array))\n"
        "            (min-element` element (cdr array))\n"
    	"			(min-element` (car array) (cdr array)) ) ) ))\n"
		"(bind min-element (func (array)\n"
	    "    (if (= (length array) 1)\n"
		"	    (car array)\n"
		"		(min-element` (car array) (cdr array)) ) ))\n"
		"(bind x (min-element [ 3 1 2 ]))";


	if (!test_source_eval(rt, source, results)) {
        return false;
	}

	if (!rt_peek_val_int(rt, results[2]) == 1) {
		printf("min-element found %ld, instead of 1.\n",
                rt_peek_val_int(rt, results[0]));
        return false;
	}

	return true;
}

int test(void)
{
	bool success = true;


	success &= test_lexer();
	success &= test_parser();

    struct Runtime *rt = rt_make(64 * 1024);
	success &= test_runtime_sanity(rt);
	success &= test_simple_algorithm(rt);
	success &= test_array_lookup(rt);
    rt_free(rt);

	if (success) {
		printf("Tests ran successfully.\n");
	}

	return !success;
}
