/* Copyright (C) 2015 Krzysztof Stachowiak */

#include "test_helpers.h"

static void test_runtime_func_simple(
        struct TestContext *tc,
        struct Runtime *rt)
{
    test_eval_source_succeed(tc, rt,
        "(bind sqr (func (x) (* x x)))\n"
        "(bind select (func (cond x y) (if cond (sqr x) (sqr y))))",
        "Declare function using another defined function");
    test_eval_source_expect(tc, rt, "(select true 2 3)", "Example AST function call 1", INT, 4);
    test_eval_source_expect(tc, rt, "(select false 2 3)", "Example AST function call 2", INT, 9);
    rt_reset(rt);
}

static void test_runtime_func_recursive(
        struct TestContext *tc,
        struct Runtime *rt)
{
    test_eval_source_succeed(tc, rt,
        "(bind gcd (func (x y) (if (= y 0) x (gcd y (% x y)) ) ))\n"
        "(bind lcm (func (x y) (* (/ x (gcd x y)) y ) ))",
        "Recursive GCD and LCM function definitions");
    test_eval_source_expect(tc, rt, "(gcd 54 24)", "GCD call", INT, 6);
    test_eval_source_expect(tc, rt, "(lcm 21 6)", "LCM call", INT, 42);
    rt_reset(rt);
}

static void test_runtime_func_simple_capture(
        struct TestContext *tc,
        struct Runtime *rt)
{
    test_eval_source_succeed(tc, rt,
        "(bind make_multiplier (func (x) (do\n"
        "    (bind lhs x)\n"
        "    (func (rhs) (* lhs rhs) )\n"
        ")))\n"
        "\n"
        "(bind doubler (make_multiplier 2))\n"
        "(bind tripler (make_multiplier 3))",
        "Definition of a functions that depend on a capture of a local value");

    test_eval_source_expect(tc, rt, "(doubler 4)", "Call to a cosure 1", INT, 8);
    test_eval_source_expect(tc, rt, "(tripler 4)", "Call to a cosure 2", INT, 12);
    rt_reset(rt);
}

void test_runtime_func(struct TestContext *tc)
{
    struct Runtime *rt = rt_make(64 * 1024);
    test_runtime_func_simple(tc, rt);
    test_runtime_func_recursive(tc, rt);
    test_runtime_func_simple_capture(tc, rt);
    rt_free(rt);
}
