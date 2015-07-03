/* Copyright (C) 2015 Krzysztof Stachowiak */

#include "test_helpers.h"

static void test_cci_impl(struct TestContext *tc, struct Runtime *rt)
{
    test_eval_source_succeed(tc, rt,
        "(bind point (func (f g x) (g (f x))))\n"
        "(bind times2 (* 2))\n"
        "(bind times4 (point times2 times2))\n"
        "(bind times6 (point times2 times4))\n",
        "Nested capture");
    test_eval_source_expect(tc, rt, "(times6 3)", "Call nested multiplier", INT, 6 * 3);
    rt_reset(rt);
}

void test_cci(struct TestContext *tc)
{
    struct Runtime *rt = rt_make();
    test_cci_impl(tc, rt);
    rt_free(rt);
}
