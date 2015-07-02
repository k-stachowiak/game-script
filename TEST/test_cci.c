/* Copyright (C) 2015 Krzysztof Stachowiak */

#include "test_helpers.h"

static void test_cci_impl(struct TestContext *tc, struct Runtime *rt)
{
    test_eval_source_fail(tc, rt,
        "(bind x 1.0)\n"
        "(bind x^ (ref x))\n"
        "(x)\n",
        "Fail on calling a value as a function after binding something else");
}

void test_cci(struct TestContext *tc)
{
    struct Runtime *rt = rt_make();
    test_cci_impl(tc, rt);
    rt_free(rt);
}
