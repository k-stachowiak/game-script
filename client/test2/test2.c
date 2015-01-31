/* Copyright (C) 2015 Krzysztof Stachowiak */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "test2_detail.h"

void tc_init(struct TestContext *tc)
{
    tc->entries.data = NULL;
    tc->entries.cap = 0;
    tc->entries.size = 0;
}

void tc_deinit(struct TestContext *tc)
{
    ARRAY_FREE(tc->entries);
}

void tc_report(struct TestContext *tc)
{
    int i, successes = 0;
    char *success_string;
    for (i = 0; i < tc->entries.size; ++i) {

        struct TestEntry *te = tc->entries.data + i;

        if (te->result) {
            ++successes;
            success_string = "OK";
        } else {
            success_string = "FAIL";
        }

        printf("[%s] %s\n", success_string, te->name);
    }

    printf("\n%d/%d tests passed.\n", successes, i);
}

void tc_record(struct TestContext *tc, char *name, bool result)
{
    struct TestEntry te = { name, result };
    ARRAY_APPEND(tc->entries, te);
}

int test2(int argc, char *argv[])
{
    struct TestContext tc;

    tc_init(&tc);
    test2_front(&tc);
    test2_runtime_basic(&tc);
    test2_runtime_func(&tc);
    tc_report(&tc);
    tc_deinit(&tc);

    return 0;
}

void test_lex(
        struct TestContext *tc,
        char *source,
        char *test_name,
        bool expect_success)
{
    struct DomNode *node = lex(source);
    tc_record(tc, test_name, !(expect_success ^ (!!node)));
    if (node) {
        dom_free(node);
    }
}

void test_parse(
        struct TestContext *tc,
        char *source,
        char *test_name,
        bool expect_success)
{
    struct AstNode *node = parse_source(source);
    tc_record(tc, test_name, !(expect_success ^ (!!node)));
    if (node) {
        ast_node_free(node);
    }
}

void test_parse_literal_string(
        struct TestContext *tc,
        char *source,
        char *test_name)
{
    struct AstNode *node = parse_source(source);
    
    if (!node) {
        tc_record(tc, test_name, false);
        return;
    }

    if (node->type != AST_LITERAL) {
        tc_record(tc, test_name, false);
        ast_node_free(node);
        return;
    }

    if (node->data.literal.type != AST_LIT_STRING) {
        tc_record(tc, test_name, false);
        ast_node_free(node);
        return;
    }
    
    if (strcmp(node->data.literal.data.string, source) != 0) {
        tc_record(tc, test_name, false);
        ast_node_free(node);
        return;
    }

    tc_record(tc, test_name, true);
    ast_node_free(node);
}

bool test_eval_source(
        struct Runtime *rt,
        char *source,
        VAL_LOC_T *locs)
{
	int i = 0;
	struct AstNode *ast_list = NULL, *next;

	err_reset();

	ast_list = parse_source(source);
	if (!ast_list) {
		return false;
	}

	while (ast_list) {
		rt_consume_one(rt, ast_list, locs + (i++), &next);
		if (err_state()) {
			ast_node_free(next);
			return false;
		}
		ast_list = next;
	}

	return true;
}

void test_eval_source_fail(
        struct TestContext *tc,
        struct Runtime *rt,
        char *source,
        char *test_name)
{
    VAL_LOC_T result[10];
    if (test_eval_source(rt, source, result)) {
        tc_record(tc, test_name, false);
    } else {
        tc_record(tc, test_name, true);
    }
}

void test_eval_source_succeed(
        struct TestContext *tc,
        struct Runtime *rt,
        char *source,
        char *test_name)
{
    VAL_LOC_T result[10];
    if (test_eval_source(rt, source, result)) {
        tc_record(tc, test_name, true);
    } else {
        tc_record(tc, test_name, false);
    }
}

void test_eval_source_expect_string(
        struct TestContext *tc,
        struct Runtime *rt,
        char *source,
        char *test_name,
        char *expected_value)
{
    VAL_LOC_T result[1];
    if (!test_eval_source(rt, source, result)) {
        tc_record(tc, test_name, false);
        rt_reset(rt);
        return;
    }
    if (*((VAL_HEAD_TYPE_T*)(rt->stack->buffer + *result)) != VAL_STRING) {
        tc_record(tc, test_name, false);
        rt_reset(rt);
        return;
    }
    if (!rt_val_string_eq(rt, *result, expected_value) != 0) {
        tc_record(tc, test_name, false);
        rt_reset(rt);
        return;
    }
    tc_record(tc, test_name, true);
    rt_reset(rt);
}

