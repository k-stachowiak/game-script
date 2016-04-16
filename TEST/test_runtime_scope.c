/* Copyright (C) 2015 Krzysztof Stachowiak */

#include "test_helpers.h"

static void test_runtime_scope_duplicate_capture(
        struct TestContext *tc,
        struct Runtime *rt)
{
    test_eval_source_succeed(tc, rt,
        "(bind foo (func (x y) (do\n"
        "    (bind bar (func (x y z)\n"
        "        (if (eq y 0)\n"
        "            z\n"
        "            (bar x (- y 1) (push_back z (x)))\n"
        "        )\n"
        "    ))\n"
        "    (bar x y [])\n"
        ")))\n"
        "\n"
        "(foo (func () (rand_ur 0.0 100.0)) 1)",
        "Test CCI");
    rt_reset(rt);
}

static void test_runtime_scope_simple(
        struct TestContext *tc,
        struct Runtime *rt)
{
    test_eval_source_succeed(tc, rt,
        "(bind a 1)\n"
        "(bind b (func (c) (do\n"
        "   (bind d 2)\n"
        "   (+ a (+ c d))\n"
        ")))",
        "Define some symbols in different scopes");
    test_eval_source_expect(tc, rt, "a", "Evaluate global symbol", INT, 1);
    test_eval_source_fail(tc, rt, "c", "Fail evaluating non-blobal symbol 1");
    test_eval_source_fail(tc, rt, "d", "Fail evaluating non-blobal symbol 1");
    test_eval_source_expect(tc, rt, "(b 3)", "Evaluate function reaching different scopes", INT, 6);
    rt_reset(rt);
}

static void test_runtime_scope_capture(
        struct TestContext *tc,
        struct Runtime *rt)
{
    test_eval_source_succeed(tc, rt,
        "(bind point (func (f g) (func (x) (g (f x)))))",
        "Nested function definition / call capture");
    rt_reset(rt);
}

void test_runtime_scope(struct TestContext *tc)
{
    struct Runtime *rt = rt_make();
    test_runtime_scope_simple(tc, rt);
    test_runtime_scope_capture(tc, rt);
    test_runtime_scope_duplicate_capture(tc, rt);
    rt_free(rt);
}
