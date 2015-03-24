/* Copyright (C) 2015 Krzysztof Stachowiak */

#include "test_helpers.h"

static void test_runtime_literals(
        struct TestContext *tc,
        struct Runtime *rt)
{
    test_eval_source_expect(tc, rt, "true", "Evaluate true literal", BOOL, true);
    test_eval_source_expect(tc, rt, "false", "Evaluate false literal", BOOL, false);
    test_eval_source_expect(tc, rt, "'a'", "Evaluate 'a' literal", CHAR, 'a');
    test_eval_source_expect(tc, rt, "'\\0'", "Evaluate '\\0' literal", CHAR, '\0');
    test_eval_source_expect(tc, rt, "1", "Evaluate 1 literal", INT, 1);
    test_eval_source_expect(tc, rt, "+2", "Evaluate +2 literal", INT, 2);
    test_eval_source_expect(tc, rt, "-3", "Evaluate -3 literal", INT, -3);
    test_eval_source_expect(tc, rt, "1.0", "Evaluate 1.0 literal", REAL, 1.0);
    test_eval_source_expect(tc, rt, "+2.0", "Evaluate +2.0 literal", REAL, 2.0);
    test_eval_source_expect(tc, rt, "-3.0", "Evaluate -3.0 literal", REAL, -3.0);
    test_eval_source_expect_string(tc, rt, "\"\"", "Evaluate empty string literal", "");
    test_eval_source_expect_string(tc, rt, "\"str\"", "Evaluate simple string literal", "str");
    test_eval_source_expect_string(tc, rt, "\"x\\\"y\"", "Evaluate string literal with escapes", "x\\\"y");
}

static void test_runtime_do(struct TestContext *tc, struct Runtime *rt)
{
    test_eval_source_fail(tc, rt, "(do)", "Fail on evaluating empty do block");
    test_eval_source_expect(tc, rt, "(do 1 2 3)", "Succeed on evaluating correct do block", INT, 3);
}

static void test_runtime_bind(struct TestContext *tc, struct Runtime *rt)
{
    test_eval_source_fail(tc, rt, "(bind)", "Fail on evaluating empty bind block");
    test_eval_source_fail(tc, rt, "(bind x)", "Fail on missing bind value");
    test_eval_source_fail(tc, rt, "(bind \"x\" 1)", "Fail on binding to literal");
    test_eval_source_expect(tc, rt, "(bind x 1.0)", "Succeed on simple bind", REAL, 1.0);
    test_eval_source_expect(tc, rt, "(bind y (+ 2 3))", "Succeed on non-trivial bind", INT, 5);
    test_eval_source_fail(tc, rt, "(bind { a } [ 1.0 ])", "Fail on mismatched types of compound bind");
    test_eval_source_fail(tc, rt, "(bind { b } { 1 2 })", "Fail on too many values in compound bind");
    test_eval_source_fail(tc, rt, "(bind [ c d ] [ 1 ])", "Fail on too many keys in compound bind");
    test_eval_source_succeed(tc, rt, "(bind { e f } { 1 2 })", "Succeed on tuple bind");
    test_eval_source_succeed(tc, rt, "(bind [ g ] [ 1.0 ])", "Succeed on array bind");
}

static void test_runtime_iff(struct TestContext *tc, struct Runtime *rt)
{
    test_eval_source_fail(tc, rt, "(if)", "Fail on evaluating empty if block");
    test_eval_source_fail(tc, rt, "(if a)", "Fail on evaluating incomplete if block 1");
    test_eval_source_fail(tc, rt, "(if a b)", "Fail on evaluating incomplete if block 2");
    test_eval_source_expect(tc, rt, "(if true 1.0 2.0)", "Succeed on iff - true branch", REAL, 1.0);
    test_eval_source_expect_string(tc, rt, "(if false \"a\" \"b\")", "Succeed on iff - false branch", "b");
}

static void test_runtime_reference(
        struct TestContext *tc,
        struct Runtime *rt)
{
    test_eval_source_fail(tc, rt, "x", "Fail on evaluating undefined reference");
    test_eval_source_expect(tc, rt, "(do (bind y 3) y)", "Succeed on evaluating defined reference", INT, 3);
}

static void test_runtime_cpd(struct TestContext *tc, struct Runtime *rt)
{
    test_eval_source_fail(tc, rt, "[ 1.0 \"two\"]", "Fail on evaluating heterogenous array");
    test_eval_source_fail(tc, rt, "[ [ 1.0 ]  [ \"two\" ] ]", "Fail on evaluating heterogenous nested array");
    test_eval_source_succeed(tc, rt, "[]", "Succeed on evaluating empty array");
    test_eval_source_succeed(tc, rt, "[ 1 2 ]", "Succeed on evaluating homogenous array");
    test_eval_source_succeed(tc, rt, "[ [ 1 ] [ 2 3 ] ]", "Succeed on evaluating homogenous nested array");
    test_eval_source_succeed(tc, rt, "{}", "Succeed on evaluating empty tuple");
    test_eval_source_succeed(tc, rt, "{ 1 2 }", "Succeed on evaluating homogenous tuple");
    test_eval_source_succeed(tc, rt, "{ 1 [] }", "Succeed on evaluating heterogenous tuple");
}

void test_runtime_basic(struct TestContext *tc)
{
	struct Runtime *rt = rt_make(64 * 1024);
    test_runtime_literals(tc, rt);
    test_runtime_do(tc, rt);
    test_runtime_bind(tc, rt);
    test_runtime_iff(tc, rt);
    test_runtime_reference(tc, rt);
    test_runtime_iff(tc, rt);
    test_runtime_cpd(tc, rt);
    rt_free(rt);
}

