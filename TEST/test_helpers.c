/* Copyright (C) 2015 Krzysztof Stachowiak */

#include "test_helpers.h"

void test_lex(
        struct TestContext *tc,
        char *source,
        char *test_name,
        bool expect_success)
{
    struct DomNode *node;

    err_reset();
    node = lex(source);
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
    struct AstNode *node;

    err_reset();
    node = parse_source(source);
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
    struct AstNode *node;

    err_reset();
    node = parse_source(source);
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
		char *error_message = err_msg();
		printf("Error in test [%s]:\n%s", test_name, error_message);
		mem_free(error_message);
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
        return;
    }
    if (!rt_val_is_string(rt, *result)) {
        tc_record(tc, test_name, false);
        return;
    }
    if (!rt_val_string_eq(rt, *result, expected_value) != 0) {
        tc_record(tc, test_name, false);
        return;
    }
    tc_record(tc, test_name, true);
}

