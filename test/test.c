/* Copyright (C) 2014,2015 Krzysztof Stachowiak */

#include <stdio.h>
#include <stdbool.h>

#include "error.h"

#include "parse.h"

#include "test.h"
#include "dom.h"
#include "runtime.h"

#include "test_detail.h"

bool test_source_eval(struct Runtime *rt, char *source, VAL_LOC_T *locs)
{
	int i = 0;
	struct AstNode *ast_list = NULL, *next;

	err_reset();

	/* 1. Parse the input source. */
	ast_list = parse_source(source);
	if (!ast_list) {
#if TEST_DEBUG
		printf("Error parsing source: %s.\n", source);
		printf("Error : %s.\n", err_msg());
#endif
		return false;
	}

	/* 2. Evaluate the source nodes. */
	while (ast_list) {
		rt_consume_one(rt, ast_list, locs + (i++), &next);
		if (err_state()) {
#if TEST_DEBUG
			printf("Failed consuming an AST node into runtime.\n");
			printf("Error : %s.\n", err_msg());
#endif
			ast_node_free(next);
			return false;
		}
		ast_list = next;
	}

	return true;
}

int test(int argc, char *argv[])
{
	struct Runtime *rt;

    TestFunction simple_tests[] = {
        test_lexer,
        test_parser
    };

    RuntimeTestFunction runtime_tests[] = {

		test_regression_comment,
        test_regression_cyclic_calls,
        test_regression_real_in_array,

        test_runtime_sanity,
        test_runtime_free_on_fail,
        test_array_homo,
        test_bif_cons_homo,
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
        printf("test %d...\n", i);
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
        printf("test %d...\n", i);
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
