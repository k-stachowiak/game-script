/* Copyright (C) 2014-2016 Krzysztof Stachowiak */

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
        "(bind gcd (func (x y) (if (eq y 0) x (gcd y (% x y)) ) ))\n"
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

static void test_runtime_func_call_non_function(
        struct TestContext *tc,
        struct Runtime *rt)
{
    test_eval_source_succeed(tc, rt,
        "(bind x 1.0)\n"
        "(bind x^ (ref x))\n",
        "Definition of a functions that depend on a capture of a local value");
    test_eval_source_fail(tc, rt, "(x)", "Fail on calling a non-funcion object as function");
    rt_reset(rt);
}

static void test_runtime_func_call_nested_currying(
        struct TestContext *tc,
        struct Runtime *rt)
{
    test_eval_source_succeed(tc, rt,
        "(bind point (func (f g x) (g (f x))))\n"
        "(bind times2 (* 2))\n"
        "(bind times4 (point times2 times2))\n"
        "(bind times8 (point times2 times4))\n",
        "Nested capture");
    test_eval_source_expect(tc, rt, "(times8 3)", "Call nested multiplier", INT, 8 * 3);
    rt_reset(rt);
}

static void test_runtime_func_call_nontrivial_call_expression(
        struct TestContext *tc,
        struct Runtime *rt)
{
    test_eval_source_succeed(tc, rt,
        "(bind x ((+ 2) 2))\n"
        "(bind custom_mul (func (x y z) (* (* x y) z)))\n"
        "(bind y (((custom_mul 1.0) 2.0 ) 3.0))\n",
        "Chained calls");
    test_eval_source_expect(tc, rt, "x", "Chained BIF call", INT, 4);
    test_eval_source_expect(tc, rt, "y", "Chained AST function call", REAL, 6.0);
    rt_reset(rt);
}

void test_runtime_func(struct TestContext *tc)
{
    struct Runtime *rt = rt_make();
    test_runtime_func_simple(tc, rt);
    test_runtime_func_recursive(tc, rt);
    test_runtime_func_simple_capture(tc, rt);
    test_runtime_func_call_non_function(tc, rt);
    test_runtime_func_call_nested_currying(tc, rt);
    test_runtime_func_call_nontrivial_call_expression(tc, rt);
    rt_free(rt);
}
