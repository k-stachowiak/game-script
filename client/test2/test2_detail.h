/* Copyright (C) 2015 Krzysztof Stachowiak */

#ifndef TEST2_DETAIL_H
#define TEST2_DETAIL_H

#include <stdbool.h>

#include "define.h"
#include "error.h"
#include "collection.h"

#include "lex.h"
#include "dom.h"
#include "parse.h"
#include "runtime.h"
#include "rt_val.h"

/* Test context API.
 * =================
 */

struct TestEntry {
    char *name;
    bool result;
};

struct TestContext {
    struct {
        struct TestEntry *data;
        int cap, size;
    } entries;
};

void tc_init(struct TestContext *tc);
void tc_deinit(struct TestContext *tc);
void tc_report(struct TestContext *tc);
void tc_record(struct TestContext *tc, char *name, bool result);

/* Main test procedures.
 * =====================
 */

void test2_front(struct TestContext *tc);
void test2_runtime_basic(struct TestContext *tc);
void test2_runtime_func(struct TestContext *tc);

/* Common lexer/parser test API.
 * =============================
 */

void test_lex(
        struct TestContext *tc,
        char *source,
        char *test_name,
        bool expect_success);
void test_parse(
        struct TestContext *tc,
        char *source,
        char *test_name,
        bool expect_success);
void test_parse_literal_string(
        struct TestContext *tc,
        char *source,
        char *test_name);

#define test_parse_literal(TC, SRC, NAME, EX_ASTTYPE, EX_CTYPE, EX_VALUE) \
    do { \
        struct AstNode *node = parse_source(SRC); \
        if (!node) { \
            tc_record(TC, NAME, false); \
            break; \
        } \
        if (node->type != AST_LITERAL) { \
            tc_record(TC, NAME, false); \
            ast_node_free(node); \
            break; \
        } \
        if (node->data.literal.type != EX_ASTTYPE) { \
            tc_record(TC, NAME, false); \
            ast_node_free(node); \
            break; \
        } \
        if (*((EX_CTYPE*)(&(node->data.literal.data))) != EX_VALUE) { \
            tc_record(TC, NAME, false); \
            ast_node_free(node); \
            break; \
        } \
        tc_record(TC, NAME, true); \
        ast_node_free(node); \
    } while (0)

/* Common runtime test api.
 * ========================
 */

bool test_eval_source(struct Runtime *rt, char *source, VAL_LOC_T *locs);
void test_eval_source_fail(
        struct TestContext *tc,
        struct Runtime *rt,
        char *source,
        char *test_name);
void test_eval_source_succeed(
        struct TestContext *tc,
        struct Runtime *rt,
        char *source,
        char *test_name);
void test_eval_source_expect_string(
        struct TestContext *tc,
        struct Runtime *rt,
        char *source,
        char *test_name,
        char *expected_value);

#define test_eval_source_expect(TC, RT, SRC, NAME, EX_TYPE, EX_VALUE) \
    do { \
        VAL_LOC_T result[1]; \
        if (!test_eval_source(RT, SRC, result)) { \
                tc_record(TC, NAME, false); \
            rt_reset(RT); \
            break; \
        } \
        if (*((VAL_HEAD_TYPE_T*)(RT->stack->buffer + *result)) != VAL_ ## EX_TYPE) { \
            tc_record(TC, NAME, false); \
            rt_reset(RT); \
            break; \
        } \
        if (*((VAL_ ## EX_TYPE ## _T*)(RT->stack->buffer + *result + VAL_HEAD_BYTES)) != EX_VALUE) { \
            tc_record(TC, NAME, false); \
            rt_reset(RT); \
            break; \
        } \
        tc_record(TC, NAME, true); \
        rt_reset(RT); \
    } while(0)

#endif

