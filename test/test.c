/* Copyright (C) 2014,2015 Krzysztof Stachowiak */

#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>

#include "error.h"
#include "test.h"
#include "dom.h"
#include "lex.h"
#include "parse.h"
#include "ast_parse.h"
#include "runtime.h"
#include "rt_val.h"

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
            printf("Failed parsing source: \"%s\".\n", good_sources[i]);
			return false;
		}
		ast_node_free(ast_node);
	}

	return true;
}

static bool test_runtime_sanity(struct Runtime *rt)
{
    VAL_LOC_T results[1];
	char *source = "(bind x \"x\")";
    return test_source_eval(rt, source, results);
}

static bool test_runtime_free_on_fail(struct Runtime *rt)
{
    char *source = "(bind x (invalid-call))";
    struct AstNode *ast_list = ast_parse_source(source);

    rt_consume_one(rt, ast_list, NULL, NULL);

    if (err_state()) {
        return true;
    } else {
        printf("No error on invalid ast consume.\n");
        return false;
    }
}

static bool test_bif_cat_homo(struct Runtime *rt)
{
    /* Allow for construction from anything and empty array. */
    char *source_good_1 = "(cons 'a' [])";

    /* Allow construction from homogenous values. */
    char *source_good_2 = "(cons 'a' [ 'b' ])";

    /* Disallow heterogenous array literals. */
    char *source_bad_1 = "[ 'a' 1 ]";

    /* Disallow construction from heterogenous values. */
    char *source_bad_2 = "(cons 'a' [ 1 ])";

    VAL_LOC_T results[1];

    if (!test_source_eval(rt, source_good_1, results)) {
        printf("Failed evaluating construction from value and empty list.\n");
        return false;
    }

    if (!test_source_eval(rt, source_good_2, results)) {
        printf("Failed evaluating construction from homogenous values.\n");
        return false;
    }

    if (test_source_eval(rt, source_bad_1, results)) {
        printf("Failed rejecting evaluation of literal of heterogenous values.\n");
        return false;
    }

    if (test_source_eval(rt, source_bad_2, results)) {
        printf("Failed rejecting construction from heterogenous value and list.\n");
        return false;
    }

    return true;
}

static bool test_local_scope(struct Runtime *rt)
{
	VAL_LOC_T results[4];
	char *source =
		"(bind sqr (func (x) (* x x)))\n"
		"(bind select (func (cond x y) (if cond (sqr x) (sqr y))))\n"
		"(bind a (select true 2 3))\n"
		"(bind b (select false 2 3))\n";

	if (!test_source_eval(rt, source, results)) {
		return false;
	}

	if (rt_val_peek_int(rt, results[2]) != 4) {
		printf("Incorrect test result.\n");
		return false;
	}

	if (rt_val_peek_int(rt, results[3]) != 9) {
		printf("Incorrect test result.\n");
		return false;
	}

	return true;
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

	if (rt_val_peek_int(rt, results[2]) != 6) {
		printf("gcd returned %" PRId64 ", instead of 6.\n", rt_val_peek_int(rt, results[0]));
        return false;
	}

	if (rt_val_peek_int(rt, results[3]) != 42) {
		printf("lcm returned %" PRId64 ", instead of 42.\n", rt_val_peek_int(rt, results[1]));
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

	if (!rt_val_peek_int(rt, results[2]) == 1) {
		printf("min-element found %" PRId64 ", instead of 1.\n",
                rt_val_peek_int(rt, results[0]));
        return false;
	}

	return true;
}

static bool test_function_object(struct Runtime *rt)
{
	VAL_LOC_T results[3];

	char *source =
		"(bind make_doubler (func () (do\n"
		"	(bind lhs 2)\n"
		"	(func (rhs) (* lhs rhs) )\n"
		")))\n"
		"\n"
		"(bind doubler (make_doubler))\n"
		"\n"
		"(bind result (doubler 4))";

	if (!test_source_eval(rt, source, results)) {
		return false;
	}

	if (!rt_val_peek_int(rt, results[2]) == 8) {
		printf("Tricky double = %" PRId64 ", instead of 8.\n",
				rt_val_peek_int(rt, results[0]));
		return false;
	}

	return true;
}

typedef bool (*TestFunction)(void);
typedef bool (*RuntimeTestFunction)(struct Runtime *);

int test(int argc, char *argv[])
{
	struct Runtime *rt;

    TestFunction simple_tests[] = {
        test_lexer,
        test_parser
    };

    RuntimeTestFunction runtime_tests[] = {
        test_runtime_sanity,
        test_runtime_free_on_fail,
        test_bif_cat_homo,
        test_local_scope,
        test_simple_algorithm,
        test_array_lookup,
        test_function_object
    };

    int simple_tests_count = sizeof(simple_tests) / sizeof(*simple_tests);
    int runtime_tests_count = sizeof(runtime_tests) / sizeof(*runtime_tests);

    int i, success;

    bool result = true;

	if (argc != 1) {
        printf("TEST: ignored additional command line arguments.\n");
    }

    /* Simple tests.
     * -------------
     */

    for (i = 0, success = 0; i < simple_tests_count; ++i) {
        if (simple_tests[i]()) {
            ++success;
        }
    }

    printf("[%s] %d/%d simple tests succeeded.\n",
        (success == simple_tests_count) ? "OK" : "FAIL",
        success, simple_tests_count
    );

    if (success != simple_tests_count) {
        result = false;
    }

    /* Runtime tests.
     * --------------
     */

	rt = rt_make(64 * 1024);

    for (i = 0, success = 0; i < runtime_tests_count; ++i) {
        if (runtime_tests[i](rt)) {
            ++success;
        }
        rt_reset(rt);
    }

    printf("[%s] %d/%d runtime tests succeeded.\n",
        (success == runtime_tests_count) ? "OK" : "FAIL",
        success, runtime_tests_count
    );

    if (success != runtime_tests_count) {
        result = false;
    }

    rt_free(rt);

	return !result;
}
