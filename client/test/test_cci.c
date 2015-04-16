/* Copyright (C) 2015 Krzysztof Stachowiak */

#include "test_helpers.h"

static void test_cci_impl(struct TestContext *tc, struct Runtime *rt)
{
    test_eval_source_succeed(tc, rt,
        "(bind sqr (func (x) (* x x)))\n"
        "(bind point (func (f g x) (g (f x))))",
        "Prepare context for test");
    test_eval_source_succeed(tc, rt,
        "(bind tetr (point sqr sqr))",
        "Defining function as composition");
    test_eval_source_expect(tc, rt,
        "(tetr 2)", "Evaluate", INT, 16);
}

void test_cci(struct TestContext *tc)
{
    struct Runtime *rt = rt_make(64 * 1024);
    test_cci_impl(tc, rt);
    rt_free(rt);
}
