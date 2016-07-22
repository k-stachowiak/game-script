/* Copyright (C) 2014-2016 Krzysztof Stachowiak */

#include "test_helpers.h"

static void test_runtime_special_logic(
        struct TestContext *tc,
        struct Runtime *rt)
{
    test_eval_source_expect(tc, rt, "(and true)", "Evaluate and 1 true arg", BOOL, true);
    test_eval_source_expect(tc, rt, "(and false)", "Evaluate and 1 false arg", BOOL, false);
    test_eval_source_expect(tc, rt, "(or true)", "Evaluate or 1 true arg", BOOL, true);
    test_eval_source_expect(tc, rt, "(or false)", "Evaluate or 1 false arg", BOOL, false);
    test_eval_source_expect(tc, rt, "(and true true)", "Evaluate and true result", BOOL, true);
    test_eval_source_expect(tc, rt, "(and false true)", "Evaluate and false result", BOOL, false);
    test_eval_source_expect(tc, rt, "(or false true)", "Evaluate or true result", BOOL, true);
    test_eval_source_expect(tc, rt, "(or false false)", "Evaluate or false result", BOOL, false);
    rt_reset(rt);
}

static void test_special_if(struct TestContext *tc, struct Runtime *rt)
{
    test_eval_source_fail(tc, rt, "(if)", "Fail on evaluating empty if block");
    test_eval_source_fail(tc, rt, "(if a)", "Fail on evaluating incomplete if block 1");
    test_eval_source_fail(tc, rt, "(if a b)", "Fail on evaluating incomplete if block 2");
    test_eval_source_expect(tc, rt, "(if true 1.0 2.0)", "Succeed on iff - true branch", REAL, 1.0);
    test_eval_source_expect_string(tc, rt, "(if false \"a\" \"b\")", "Succeed on iff - false branch", "b");
    rt_reset(rt);
}

static void test_special_references_basic(struct TestContext *tc, struct Runtime *rt)
{
    test_eval_source_succeed(tc, rt,
            "(bind x 1)\n"
            "(bind x^ (ref x))\n"
            "(bind y (peek x^))\n"
            "(poke x^ 2)",
            "Mutate atomic value through reference");
    test_eval_source_expect(tc, rt, "x", "Value mutated through reference", INT, 2);
    test_eval_source_expect(tc, rt, "y", "Copy through reference persisted", INT, 1);
    rt_reset(rt);
}

static void test_special_references_iteration(struct TestContext *tc, struct Runtime *rt)
{
    test_eval_source_succeed(tc, rt,
            "(bind vec [ 1 2 3 ])\n"
            "(bind x^ (begin vec))\n"
            "(bind x (peek x^))\n"
            "(bind y^ (succ x^))\n"
            "(bind y (peek y^))\n"
            "(bind z^ (succ y^))\n"
            "(bind z (peek z^))\n",
            "Iterate over vector manually");
    test_eval_source_expect(tc, rt, "x", "First value read correctly", INT, 1);
    test_eval_source_expect(tc, rt, "y", "Second value read correctly", INT, 2);
    test_eval_source_expect(tc, rt, "z", "Third value read correctly", INT, 3);
    rt_reset(rt);
}

static void test_special_references_poke_type(struct TestContext *tc, struct Runtime *rt)
{
    test_eval_source_succeed(tc, rt,
            "(bind v [ 1 2 3 ])\n"
            "(bind v^ (ref v))\n"
            "(bind x 4.0)\n"
            "(bind x^ (ref x))",
            "Mutate different types of objects");

    test_eval_source_fail(tc, rt, "(poke v^ [ 1 2 ])", "Fail on poking array with size mismatch");
    test_eval_source_fail(tc, rt, "(poke v^ [ 'a' 'b' 'c' ])", "Fail on poking array of mismatched type elements");
    test_eval_source_fail(tc, rt, "(poke v^ 1.0)", "Fail on poking array with an atomic value");
    test_eval_source_succeed(tc, rt, "(poke v^ [ 3 2 1 ])", "Succeed with poking an array");
    test_eval_source_expect(tc, rt, "(at v 0)", "Array poke successful", INT, 3);
    test_eval_source_expect(tc, rt, "(at v 1)", "Array poke successful", INT, 2);
    test_eval_source_expect(tc, rt, "(at v 2)", "Array poke successful", INT, 1);

    test_eval_source_fail(tc, rt, "(poke x^ [ 1 2 3 ])", "Fail on poking atomic value with an array");
    test_eval_source_fail(tc, rt, "(poke x^ [])", "Fail on poking atomic value with an empty array");
    test_eval_source_fail(tc, rt, "(poke x^ 1)", "Fail on poking atomic value with mistmatched type");
    test_eval_source_succeed(tc, rt, "(poke x^ 2.0)", "Succeed with poking an atomic value");
    test_eval_source_expect(tc, rt, "x", "Value poke successful", REAL, 2.0);

    rt_reset(rt);
}
void test_runtime_special(struct TestContext *tc)
{
    struct Runtime *rt = rt_make();
    test_runtime_special_logic(tc, rt);
    test_special_if(tc, rt);
    test_special_references_basic(tc, rt);
    test_special_references_iteration(tc, rt);
    test_special_references_poke_type(tc, rt);
    rt_free(rt);
}
