/* Copyright (C) 2015 Krzysztof Stachowiak */

#include "test_helpers.h"

static void test_runtime_bif_arythmetic(
        struct TestContext *tc,
        struct Runtime *rt)
{
    test_eval_source_expect(tc, rt, "(+ 2 2)", "Evaluate + homogenous args", INT, 4);
    test_eval_source_expect(tc, rt, "(/ 2.0 2.0)", "Evaluate / homogenous args", REAL, 1.0);
    test_eval_source_expect(tc, rt, "(- 2 (* 2 2))", "Evaluate - and *", INT, -2);
    test_eval_source_expect(tc, rt, "(* 2.0 (+ 2.0 2.0))", "Evaluate * and +", REAL, 8.0);
    test_eval_source_expect(tc, rt, "(+ (/ 7 2) (% 7 2))", "Evaluate 7 / 2 + 7 % 2 (int)", INT, 4);
    test_eval_source_expect(tc, rt, "(+ (/ 7.0 2.0) (% 7.0 2.0))", "Evaluate 7 / 2 + 7 % 2 (real)", REAL, 4.5);
    test_eval_source_expect(tc, rt, "(/ -2.0 2.0)", "Evaluate operation on negative value", REAL, -1.0);
    test_eval_source_expect(tc, rt, "(sqrt (+ (* 3.0 3.0) (* 4.0 4.0)))", "Evaluate egyptian diagonal", REAL, 5.0);
    test_eval_source_fail(tc, rt, "(- 2.0 2)", "Evaluate - heterogenous args");
    test_eval_source_fail(tc, rt, "(* 2 2.0)", "Evaluate * heterogenous args");
}

static void test_runtime_bif_logic(
        struct TestContext *tc,
        struct Runtime *rt)
{
    test_eval_source_expect(tc, rt, "(^^ false true)", "Evaluate || true result", BOOL, true);
    test_eval_source_expect(tc, rt, "(^^ true true)", "Evaluate || false result", BOOL, false);
    test_eval_source_expect(tc, rt, "(~~ false)", "Evaluate ~~ false result", BOOL, false);
}

static void test_runtime_bif_compare(
        struct TestContext *tc,
        struct Runtime *rt)
{
    test_eval_source_expect(tc, rt, "(= 2 2)", "Evaluate equality true", BOOL, true);
    test_eval_source_expect(tc, rt, "(= 12.0 23.0)", "Evaluate equality false", BOOL, false);
    test_eval_source_expect(tc, rt, "(< 12.0 23.0)", "Evaluate less than true", BOOL, true);
    test_eval_source_expect(tc, rt, "(< 2 2)", "Evaluate less than false", BOOL, false);
    test_eval_source_expect(tc, rt, "(> 23.0 12.0)", "Evaluate greater than true", BOOL, true);
    test_eval_source_expect(tc, rt, "(> 2 2)", "Evaluate greater than false", BOOL, false);
    test_eval_source_expect(tc, rt, "(<= 12.0 23.0)", "Evaluate less than or equal true", BOOL, true);
    test_eval_source_expect(tc, rt, "(<= 2 2)", "Evaluate less than or equal true", BOOL, true);
    test_eval_source_expect(tc, rt, "(<= 2 1)", "Evaluate less than or equal false", BOOL, false);
    test_eval_source_expect(tc, rt, "(>= 12.0 23.0)", "Evaluate greater than or equal false", BOOL, false);
    test_eval_source_expect(tc, rt, "(>= 2 2)", "Evaluate greater than or equal true", BOOL, true);
    test_eval_source_expect(tc, rt, "(>= 2 1)", "Evaluate greater than or equal true", BOOL, true);
}

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

static void test_runtime_bif_text(
        struct TestContext *tc,
        struct Runtime *rt)
{
    test_eval_source_fail(tc, rt, "(printf \"%d%f\" { 1 })", "Printf wit not enough args");
    test_eval_source_fail(tc, rt, "(printf \"%d\" { 1.0 })", "Printf with incorrect arg type");
    test_eval_source_fail(tc, rt, "(printf \"%d\" { 1 1.0 })", "Printf with too many args");

    test_eval_source_expect(tc, rt,
        "(printf \"%s, %s!\\n\" { \"Hello\" \"World\" } )",
        "Printing format with two strings",
        INT, strlen("Hello, World!\n"));

    test_eval_source_expect(tc, rt,
        "(printf \"%d%d\" { 1 2 } )",
        "Printing format with consecutive wildcards",
        INT, 2);
}

void test_runtime_bif(struct TestContext *tc)
{
    struct Runtime *rt = rt_make(64 * 1024);
    test_runtime_bif_arythmetic(tc, rt);
    test_runtime_bif_logic(tc, rt);
    test_runtime_bif_compare(tc, rt);
    test_runtime_bif_cons_valid(tc, rt);
    test_runtime_bif_cons_invalid(tc, rt);
    test_runtime_bif_cat_valid(tc, rt);
    test_runtime_bif_cat_invalid(tc, rt);
    test_runtime_bif_text(tc, rt);
    rt_free(rt);
}

