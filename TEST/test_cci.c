/* Copyright (C) 2015-2016 Krzysztof Stachowiak */

#include "test_helpers.h"
#include "sexpr_tools.h"

static void test_cci_impl(struct TestContext *tc, struct Runtime *rt)
{
    (void)tc;
    (void)rt;
}

void test_cci(struct TestContext *tc)
{
    struct Runtime *rt = rt_make();
    test_cci_impl(tc, rt);
    rt_free(rt);
}
