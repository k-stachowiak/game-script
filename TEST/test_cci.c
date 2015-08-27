/* Copyright (C) 2015 Krzysztof Stachowiak */

#include "test_helpers.h"

static void test_cci_impl(struct TestContext *tc, struct Runtime *rt)
{
    struct AstNode *node;

    (void)rt;
    (void)tc;

    node = parse_source("(bind (a) 1)");
    if (node) {
        ast_node_free(node);
    }
}

void test_cci(struct TestContext *tc)
{
    struct Runtime *rt = rt_make();
    test_cci_impl(tc, rt);
    rt_free(rt);
}
