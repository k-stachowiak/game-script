/* Copyright (C) 2015 Krzysztof Stachowiak */

#include "test_helpers.h"

static void test_runtime_func_simple(
        struct TestContext *tc,
        struct Runtime *rt)
{
    VAL_LOC_T results[4];

    char *test_name = "Simple function definition and calls";
    char *source =
        "(bind sqr (func (x) (* x x)))\n"
        "(bind select (func (cond x y) (if cond (sqr x) (sqr y))))\n"
        "(bind a (select true 2 3)) # Expected to be 4\n"
        "(bind b (select false 2 3)) # Expected to be 9\n";

    if (!test_eval_source(rt, source, results)) {
        tc_record(tc, test_name, false);
        rt_reset(rt);
        return;
    }

    if (rt_val_peek_int(rt, results[2]) != 4) {
        tc_record(tc, test_name, false);
        rt_reset(rt);
        return;
    }

    if (rt_val_peek_int(rt, results[3]) != 9) {
        tc_record(tc, test_name, false);
        rt_reset(rt);
        return;
    }

    tc_record(tc, test_name, true);
    rt_reset(rt);
}

static void test_runtime_func_recursive(
        struct TestContext *tc,
        struct Runtime *rt)
{
    VAL_LOC_T results[4];

    char *test_name = "Recursive GCD and LCM function definition and calls";
    char *source =
        "(bind gcd (func (x y) (if (= y 0) x (gcd y (% x y)) ) ))\n"
        "(bind lcm (func (x y) (* (/ x (gcd x y)) y ) ))\n"
        "(bind x (gcd 54 24)) # Expected to be 6\n"
        "(bind y (lcm 21 6)) # Expected to be 42";

    if (!test_eval_source(rt, source, results)) {
        tc_record(tc, test_name, false);
        rt_reset(rt);
        return;
    }

    if (rt_val_peek_int(rt, results[2]) != 6) {
        tc_record(tc, test_name, false);
        rt_reset(rt);
        return;
    }

    if (rt_val_peek_int(rt, results[3]) != 42) {
        tc_record(tc, test_name, false);
        rt_reset(rt);
        return;
    }

    tc_record(tc, test_name, true);
    rt_reset(rt);
}

static void test_runtime_func_simple_capture(
        struct TestContext *tc,
        struct Runtime *rt)
{
    VAL_LOC_T results[5];

    char *test_name = "Function object creation and use";
    char *source =
        "(bind make_multiplier (func (x) (do\n"
        "    (bind lhs x)\n"
        "    (func (rhs) (* lhs rhs) )\n"
        ")))\n"
        "\n"
        "(bind doubler (make_multiplier 2))\n"
        "(bind tripler (make_multiplier 3))\n"
        "\n"
        "(bind x (doubler 4)) # Expected to be 8\n"
        "(bind y (tripler 4)) # Expected to be 12";

    if (!test_eval_source(rt, source, results)) {
        tc_record(tc, test_name, false);
        rt_reset(rt);
        return;
    }

    if (rt_val_peek_int(rt, results[3]) != 8) {
        tc_record(tc, test_name, false);
        rt_reset(rt);
        return;
    }

    if (rt_val_peek_int(rt, results[4]) != 12) {
        tc_record(tc, test_name, false);
        rt_reset(rt);
        return;
    }

    tc_record(tc, test_name, true);
    rt_reset(rt);
}

static void test_runtime_func_cyclic_capture(
        struct TestContext *tc,
        struct Runtime *rt)
{
    VAL_LOC_T results[3];

    char *test_name = "Convoluted capture / function call";
    char *source =
        "(bind cross-call (func (x) (do\n"
    	"    (bind > (func (x y) (&& (~~ (< x y)) (~~ (= x y)))))\n"
        "    (bind call-ping (func (x)\n"
        "        (if (> x 0)\n"
        "            (call-pong (- x 1))\n"
        "            \"success ping\"\n"
        "        )\n"
        "    ))\n"
        "    (bind call-pong (func (x)\n"
        "        (if (> x 0)\n"
        "            (call-ping (- x 1))\n"
        "            \"success pong\"\n"
        "        )\n"
        "    ))\n"
        "    (call-ping x)\n"
        ")))\n"
        "(bind x (cross-call 2))\n"
        "(bind y (cross-call 3))";

    if (!test_eval_source(rt, source, results)) {
        tc_record(tc, test_name, false);
        rt_reset(rt);
        return;
    }

    if (!rt_val_string_eq(rt, results[1], "success ping")) {
        tc_record(tc, test_name, false);
        rt_reset(rt);
        return;
    }

    if (!rt_val_string_eq(rt, results[2], "success pong")) {
        tc_record(tc, test_name, false);
        rt_reset(rt);
        return;
    }

    tc_record(tc, test_name, true);
    rt_reset(rt);
}

void test_runtime_func(struct TestContext *tc)
{
    struct Runtime *rt = rt_make(64 * 1024);
    test_runtime_func_simple(tc, rt);
    test_runtime_func_recursive(tc, rt);
    test_runtime_func_simple_capture(tc, rt);
    test_runtime_func_cyclic_capture(tc, rt);
    rt_free(rt);
}
