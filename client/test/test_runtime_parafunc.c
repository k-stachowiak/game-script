/* Copyright (C) 2015 Krzysztof Stachowiak */

#include "test_helpers.h"

static void test_runtime_parafunc_logic(
        struct TestContext *tc,
        struct Runtime *rt)
{
    test_eval_source_expect(tc, rt, "(and true true)", "Evaluate and true result", BOOL, true);
    test_eval_source_expect(tc, rt, "(and false true)", "Evaluate and false result", BOOL, false);
    test_eval_source_expect(tc, rt, "(or false true)", "Evaluate or true result", BOOL, true);
    test_eval_source_expect(tc, rt, "(or false false)", "Evaluate or false result", BOOL, false);
    rt_reset(rt);
}

static void test_parafunc_if(struct TestContext *tc, struct Runtime *rt)
{
    test_eval_source_fail(tc, rt, "(if)", "Fail on evaluating empty if block");
    test_eval_source_fail(tc, rt, "(if a)", "Fail on evaluating incomplete if block 1");
    test_eval_source_fail(tc, rt, "(if a b)", "Fail on evaluating incomplete if block 2");
    test_eval_source_expect(tc, rt, "(if true 1.0 2.0)", "Succeed on iff - true branch", REAL, 1.0);
    test_eval_source_expect_string(tc, rt, "(if false \"a\" \"b\")", "Succeed on iff - false branch", "b");
    rt_reset(rt);
}

static void test_parafunc_switch(struct TestContext *tc, struct Runtime *rt)
{
    test_eval_source_fail(tc, rt, "(switch)", "Fail on evaluating empty switch block");
    test_eval_source_fail(tc, rt, "(switch 1)", "Fail on evaluating switch block with only 1 argument");
    test_eval_source_fail(tc, rt, "(switch 1 2)", "Fail on evaluating switch block with a non-tuple case");
    test_eval_source_fail(tc, rt, "(switch 1 { 2 })", "Fail on evaluating switch block with a 1-element");
    test_eval_source_expect(tc, rt, "(switch 1 { 1 1.0 } { 2 2.0 })", "Succeed on switch a", REAL, 1.0);
    test_eval_source_expect(tc, rt, "(switch 2 { 1 1.0 } { 2 2.0 })", "Succeed on switch b", REAL, 2.0);
    rt_reset(rt);
}

void test_runtime_parafunc(struct TestContext *tc)
{
    struct Runtime *rt = rt_make();
    test_runtime_parafunc_logic(tc, rt);
    test_parafunc_if(tc, rt);
    test_parafunc_switch(tc, rt);
    rt_free(rt);
}
