/* Copyright (C) 2015 Krzysztof Stachowiak */

#include "test_helpers.h"

static void test_runtime_bif_cons_valid(
        struct TestContext *tc,
        struct Runtime *rt)
{
    VAL_LOC_T results[4];

    char *test_name = "BIF cons valid";
    char *source =
        "(bind x (cons 'a' []))\n"
        "(bind y (cons 1 [ 2 3 ]))\n"
        "(bind x-len (length x))\n"
        "(bind y-len (length y))";

    if (!test_eval_source(rt, source, results)) {
        tc_record(tc, test_name, false);
        rt_reset(rt);
        return;
    }

	if (rt_val_peek_int(rt, results[2]) != 1) {
        tc_record(tc, test_name, false);
        rt_reset(rt);
        return;
    }

	if (rt_val_peek_int(rt, results[3]) != 3) {
        tc_record(tc, test_name, false);
        rt_reset(rt);
        return;
    }

    tc_record(tc, test_name, true);
    rt_reset(rt);
}

static void test_runtime_bif_cons_invalid(
        struct TestContext *tc,
        struct Runtime *rt)
{
    test_eval_source_fail(tc, rt, "(cons [ 1 ] [ 2 ])", "BIF cons two array args");
    test_eval_source_fail(tc, rt, "(cons 1 2)", "BIF cons no array args");
    test_eval_source_fail(tc, rt, "(cons 1.0 [ 2 ])", "BIF cons hetero args");
}

static void test_runtime_bif_cat_valid(
        struct TestContext *tc,
        struct Runtime *rt)
{
    VAL_LOC_T results[6];

    char *test_name = "BIF cons valid";
    char *source =
        "(bind x (cat [] []))\n"
        "(bind y (cat [ \"\" ] []))\n"
        "(bind z (cat [ 1 ] [ 2 3 ]))\n"
        "(bind x-len (length x))\n"
        "(bind y-len (length y))\n"
        "(bind z-len (length z))";

    if (!test_eval_source(rt, source, results)) {
        tc_record(tc, test_name, false);
        rt_reset(rt);
        return;
    }

	if (rt_val_peek_int(rt, results[3]) != 0) {
        tc_record(tc, test_name, false);
        rt_reset(rt);
        return;
    }

	if (rt_val_peek_int(rt, results[4]) != 1) {
        tc_record(tc, test_name, false);
        rt_reset(rt);
        return;
    }

	if (rt_val_peek_int(rt, results[5]) != 3) {
        tc_record(tc, test_name, false);
        rt_reset(rt);
        return;
    }

    tc_record(tc, test_name, true);
    rt_reset(rt);
}

static void test_runtime_bif_cat_invalid(
        struct TestContext *tc,
        struct Runtime *rt)
{
    test_eval_source_fail(tc, rt, "(cat 1 2)", "BIF cat two non-array args");
    test_eval_source_fail(tc, rt, "(cat [ 1 ] 2)", "BIF cat one no-array args");
    test_eval_source_fail(tc, rt, "(cat 1 [ 2 ])", "BIF cat one no-array args");
    test_eval_source_fail(tc, rt, "(cat [ 1.0 ] [ 2 ])", "BIF cat hetero args");
}

void test_runtime_bif(struct TestContext *tc)
{
	struct Runtime *rt = rt_make(64 * 1024);
    test_runtime_bif_cons_valid(tc, rt);
    test_runtime_bif_cons_invalid(tc, rt);
    test_runtime_bif_cat_valid(tc, rt);
    test_runtime_bif_cat_invalid(tc, rt);
    rt_free(rt);
}
