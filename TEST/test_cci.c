/* Copyright (C) 2015 Krzysztof Stachowiak */

#include "test_helpers.h"

static void test_cci_impl(struct TestContext *tc, struct Runtime *rt)
{
    char *source =
	"(bind array_gen (func (f n) (do\n"
	"    (bind array_gen_impl (func (f n acc)\n"
        "        (if (eq n 0)\n"
        "            acc\n"
        "            (array_gen_impl f (- n 1) (push_back acc (f)))\n"
        "        )\n"
        "    ))\n"
        "    (array_gen_impl f n [])\n"
        ")))\n"
        "\n"
        "(bind array500 (array_gen (func () (rand_ur 0.0 100.0)) 500))";
    test_eval_source_succeed(tc, rt, source, "Test CCI");
}

void test_cci(struct TestContext *tc)
{
    struct Runtime *rt = rt_make();
    test_cci_impl(tc, rt);
    rt_free(rt);
}
