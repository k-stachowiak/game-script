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
    node = parse_source(source, NULL, NULL, NULL);
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
    node = parse_source(source, NULL, NULL, NULL);
    if (!node) {
        tc_record(tc, test_name, false);
        return;
    }

    if (node->type != AST_LITERAL_ATOMIC) {
        tc_record(tc, test_name, false);
        ast_node_free(node);
        return;
    }

    if (node->data.literal_atomic.type != AST_LIT_ATOM_STRING) {
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
    struct AstLocMap alm;

    err_reset();
    alm_init(&alm);

    ast_list = parse_source_build_alm(source, &alm);
    if (!ast_list) {
	alm_deinit(&alm);
        return false;
    }

    while (ast_list) {
        rt_consume_one(rt, ast_list, &alm, locs + (i++), &next);
        if (err_state()) {
            ast_node_free(next);
	    alm_deinit(&alm);
            return false;
        }
        ast_list = next;
    }

    alm_deinit(&alm);
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

