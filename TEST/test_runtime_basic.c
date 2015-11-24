
/* Copyright (C) 2015 Krzysztof Stachowiak */

#include "test_helpers.h"

static void test_runtime_literals(
        struct TestContext *tc,
        struct Runtime *rt)
{
    /* Pretty special test case for the unit literal. */
    VAL_LOC_T top_before, top_after;
    top_before = rt->stack.top;
    test_eval_source_succeed(tc, rt, "unit", "Don't fail on unit");
    top_after = rt->stack.top;
    tc_record(tc, "Don't push on unit", top_before == top_after);

    /* Simple tests for the evaluation of regular literals. */
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
    test_eval_source_expect_string(tc, rt, "\"x\\\"y\"", "Evaluate string literal with escapes", "x\"y");
    test_eval_source_expect_string(tc, rt, "\"\\n\"", "Evaluate string literal with escaped whitespace", "\n");
    rt_reset(rt);
}

static void test_runtime_do(struct TestContext *tc, struct Runtime *rt)
{
    test_eval_source_fail(tc, rt, "(do)", "Fail on evaluating empty do block");
    test_eval_source_expect(tc, rt, "(do 1 2 3)", "Succeed on evaluating correct do block", INT, 3);
    rt_reset(rt);
}

static void test_runtime_bind(struct TestContext *tc, struct Runtime *rt)
{
    /* Alias binds */
    test_eval_source_fail(tc, rt, "(bind \"x\" 1)", "Fail on binding to literal");
    test_eval_source_expect(tc, rt, "(bind x 1.0)", "Succeed on simple bind", REAL, 1.0);
    test_eval_source_expect(tc, rt, "(bind y (+ 2 3))", "Succeed on non-trivial bind", INT, 5);

    /* Compound binds */
    test_eval_source_fail(tc, rt, "(bind {} 1.0)", "Fail on binding to empty tuple");
    test_eval_source_fail(tc, rt, "(bind { a [] } 1.0 [ 1 2 ])", "Fail on binding to empty array");
    test_eval_source_fail(tc, rt, "(bind 1 2)", "Fail on mismatched simple literals");
    test_eval_source_fail(tc, rt, "(bind [ 1 2 ] [ 2 1 ])", "Fail on mismatched complex literals");
    test_eval_source_fail(tc, rt, "(bind { a } [ 1.0 ])", "Fail on mismatched types of compound bind");
    test_eval_source_fail(tc, rt, "(bind { b } { 1 2 })", "Fail on too many values in compound bind");
    test_eval_source_fail(tc, rt, "(bind [ c d ] [ 1 ])", "Fail on too many keys in compound bind");
    test_eval_source_succeed(tc, rt, "(bind 1.0 1.0)", "Succeed on matched simple literals");
    test_eval_source_succeed(tc, rt, "(bind [ 2 3 ] [ (+ 1 1) 3 ])", "Succeed on matched complex literals");
    test_eval_source_succeed(tc, rt, "(bind \"asd\" [ 'a' 's' 'd' ])", "Succeed on matching array to string");
    test_eval_source_succeed(tc, rt, "(bind [ 'a' 's' 'd' ] \"asd\")", "Succeed on matching string to array");
    test_eval_source_succeed(tc, rt, "(bind { e f } { 1 2 })", "Succeed on tuple bind");
    test_eval_source_succeed(tc, rt, "(bind [ g ] [ 1.0 ])", "Succeed on array bind");
    test_eval_source_expect(tc, rt, "(do (bind { s _ } { 1.0 2.0 }) s)", "Bind tuple 1st dontcare 2nd", REAL, 1.0);
    test_eval_source_expect(tc, rt, "(do (bind { _ t } { 1.0 2.0 }) t)", "Bind tuple 2nd dontcare 1st", REAL, 2.0);

    rt_reset(rt);
}

static void test_runtime_reference(
        struct TestContext *tc,
        struct Runtime *rt)
{
    test_eval_source_fail(tc, rt, "x", "Fail on evaluating undefined reference");
    test_eval_source_expect(tc, rt, "(do (bind y 3) y)", "Succeed on evaluating defined reference", INT, 3);
    rt_reset(rt);
}

static void test_runtime_cpd(struct TestContext *tc, struct Runtime *rt)
{
    test_eval_source_fail(tc, rt, "[ 1.0 \"two\"]", "Fail on evaluating heterogenous array");
    test_eval_source_fail(tc, rt, "[ [ 1.0 ]  [ \"two\" ] ]", "Fail on evaluating heterogenous nested array");
    test_eval_source_fail(tc, rt, "[ \"tic\"  \"tack\" ]", "Fail on evaluating heterogenous nested array");
    test_eval_source_fail(tc, rt, "[ { 1 } { 2 3 } ]", "Fail on evaluating heterogenous array of tuples");
    test_eval_source_succeed(tc, rt, "[]", "Succeed on evaluating empty array");
    test_eval_source_succeed(tc, rt, "[ 1 2 ]", "Succeed on evaluating homogenous array");
    test_eval_source_succeed(tc, rt, "[ [ 1 2 ] [ 3 4 ] ]", "Succeed on evaluating homogenous nested array");
    test_eval_source_succeed(tc, rt, "[ { 1 2 } { 3 4 } ]", "Succeed on evaluating homogenous array of tuples");
    test_eval_source_succeed(tc, rt, "{}", "Succeed on evaluating empty tuple");
    test_eval_source_succeed(tc, rt, "{ 1 2 }", "Succeed on evaluating homogenous tuple");
    test_eval_source_succeed(tc, rt, "{ 1 [] }", "Succeed on evaluating heterogenous tuple");
    rt_reset(rt);
}

void test_runtime_basic(struct TestContext *tc)
{
    struct Runtime *rt = rt_make();
    test_runtime_literals(tc, rt);
    test_runtime_do(tc, rt);
    test_runtime_bind(tc, rt);
    test_runtime_reference(tc, rt);
    test_runtime_cpd(tc, rt);
    rt_free(rt);
}

