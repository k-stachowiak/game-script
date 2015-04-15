/* Copyright (C) 2015 Krzysztof Stachowiak */

#include "test_helpers.h"

static void test_cci_impl(struct TestContext *tc, struct Runtime *rt)
{

}

void test_cci(struct TestContext *tc)
{
    struct Runtime *rt = rt_make(64 * 1024);
    test_cci_impl(tc, rt);
    rt_free(rt);
}
