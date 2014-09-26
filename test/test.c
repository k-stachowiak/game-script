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

static bool eval_source(char *source,
						int result_offset,
						struct Value *values)
{
	int i = 0, ast_len;
	struct AstNode *ast_list = NULL, *ast_current;
	VAL_LOC_T *locations = NULL;
	bool result = true;

	err_reset();

	/* 1. Parse the input source. */
	ast_list = ast_parse_source(source);
	if (!ast_list) {
		result = false;
		goto end;
	}

	ast_len = ast_list_len(ast_list);
	ast_current = ast_list;

	/* 2. Evaluate the source nodes. */
	rt_init();
	locations = malloc_or_die(ast_len * sizeof(*locations));
	while (ast_current) {
		locations[i++] = rt_consume_one(ast_current);
		if (err_state()) {
			printf("Failed consuming an AST node into runtime.\n");
			printf("Error : %s.\n", err_msg());
			result = false;
			goto end;
		}
		ast_current = ast_current->next;
	}

	/* 3. Retrieve values from the runtime. */
	for (i = 0; i < (ast_len - result_offset); ++i) {
		values[i] = rt_peek(locations[result_offset + i]);
	}

end:
	if (ast_list) {
		ast_node_free(ast_list);
	}

	if (locations) {
		free(locations);
	}

	rt_deinit();

	return result;
}

static bool parser_test()
{
	int i;

	char *good_sources[] = {
		"a\nb",                           /* Lack of whitespaces, */
		"(do (bind x \"ex\") (cat x x))", /* parsing DO, BIND and func call, */
		"(if true 1 2)",                  /* parsing IF, */
		"[ 1 { 1.0 1.0 [ 'a' 'b' ] } ]",  /* parsing nested array and tuple, */
		"(func (x) (* x x))",             /* parsing function definition, */
		"{ true \"as\" '\\t' 6.0 1 x }",  /* parsing some literals. */
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

static bool simple_algorithm_test(void)
{
	char *source =
		"(bind gcd (func (x y) (if (= y 0) x (gcd y (% x y)) ) ))\n"
		"(bind lcm (func (x y) (* (/ x (gcd x y)) y ) ))\n"
		"(bind x (gcd 54 24)) # Expected to be 6\n"
		"(bind y (lcm 21 6)) # Expected to be 42";

	struct Value results[2];

	if (!eval_source(source, 2, results)) {
		return false;
	}

	if (!val_eq_int(results + 0, 6)) {
		printf("gcd returned %ld, instead of 6.\n",
			   (long)results[0].primitive.integer);
		return false;
	}

	if (!val_eq_int(results + 1, 42)) {
		printf("lcm returned %ld, instead of 42.\n",
			   (long)results[1].primitive.integer);
		return false;
	}

	return true;
}

static bool array_lookup_test(void)
{
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

	struct Value results[1];

	if (!eval_source(source, 2, results)) {
		return false;
	}

	if (!val_eq_int(results + 0, 1)) {
		printf("min-element found %ld, instead of 1.\n",
			   (long)results[0].primitive.integer);
		return false;
	}

	return true;
}

int test(void)
{
	int result = 0;

	if (/*!lexer_test() ||
		!parser_test() ||
		*/		!simple_algorithm_test() /*||
		!array_lookup_test()*/) {
		result = 1;
	}

	if (result == 0) {
		printf("Tests ran successfully.\n");
	}

	return result;
}
