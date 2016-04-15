/* Copyright (C) 2015-2016 Krzysztof Stachowiak */

#include "test_helpers.h"
#include "sexpr_tools.h"

static void test_cci_impl(struct TestContext *tc, struct Runtime *rt)
{
    char *source =
        "(bind foo (func (x y) (do\n"
        "    (bind bar (func (x y z)\n"
        "        (if (eq y 0)\n"
        "            z\n"
        "            (bar x (- y 1) (push_back z (f)))\n"
        "        )\n"
        "    ))\n"
        "    (bar x y [])\n"
        ")))\n"
        "\n"
        "(foo (func () (rand_ur 0.0 100.0)) 1)";
    test_eval_source_succeed(tc, rt, source, "Test CCI");
}

void test_cci(struct TestContext *tc)
{
    struct Runtime *rt = rt_make();
    test_cci_impl(tc, rt);
    rt_free(rt);
}
