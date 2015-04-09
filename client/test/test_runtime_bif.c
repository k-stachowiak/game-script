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
    test_eval_source_expect(tc, rt, "(- 2.0 2)", "Evaluate - heterogenous args", REAL, 0.0);
    test_eval_source_expect(tc, rt, "(* 2 2.0)", "Evaluate * heterogenous args", REAL, 4.0);
    test_eval_source_expect(tc, rt, "(floor -3.5)", "Evaluate floor", INT, -4);
    test_eval_source_expect(tc, rt, "(ceil -3.5)", "Evaluate ceil", INT, -3);
    test_eval_source_expect(tc, rt, "(round 3.5)", "Evaluate round", INT, 4);
}

static void test_runtime_bif_logic(
        struct TestContext *tc,
        struct Runtime *rt)
{
    test_eval_source_expect(tc, rt, "(^^ false true)", "Evaluate || true result", BOOL, true);
    test_eval_source_expect(tc, rt, "(^^ true true)", "Evaluate || false result", BOOL, false);
    test_eval_source_expect(tc, rt, "(~~ false)", "Evaluate ~~ false result", BOOL, false);
    test_eval_source_expect(tc, rt, "(&& true true false)", "Evaluate &&", BOOL, false);
    test_eval_source_expect(tc, rt, "(|| false true false)", "Evaluate ||", BOOL, true);
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

static void test_runtime_bif_length(
        struct TestContext *tc,
        struct Runtime *rt)
{
    test_eval_source_expect(tc, rt, "(length [])", "Evaluate length of an empty array", INT, 0);
    test_eval_source_expect(tc, rt, "(length [ 'a' 'b' 'c' ])", "Evaluate length of an non-empty array", INT, 3);
    test_eval_source_fail(tc, rt, "(length \"asdf\")", "Fail on length of a non-array");
}

static void test_runtime_bif_at(
        struct TestContext *tc,
        struct Runtime *rt)
{
    test_eval_source_expect(tc, rt, "(at [ 1 2 3 ] 0)", "Evaluate valid at call", INT, 1);
    test_eval_source_fail(tc, rt, "(at [ 1 ] 0 0)", "Fail at on too many arguments");
    test_eval_source_fail(tc, rt, "(at [] 1.0)", "Fail at on non-integer index");
    test_eval_source_fail(tc, rt, "(at 1.0 1)", "Fail at on non-array subject");
    test_eval_source_fail(tc, rt, "(at [ \"alpha\" \"beta\" \"gamma\" ] -1)", "Fail at on negative index");
    test_eval_source_fail(tc, rt, "(at [ \"alpha\" \"beta\" \"gamma\" ] 3)", "Fail at on index == size");
    test_eval_source_fail(tc, rt, "(at [ \"alpha\" \"beta\" \"gamma\" ] 300)", "Fail at on index beyond size");
}

static void test_runtime_bif_reverse(
        struct TestContext *tc,
        struct Runtime *rt)
{
    test_eval_source_expect(tc, rt, "(= (reverse [ 'b' 'c' 'a' ]) [ 'a' 'c' 'b' ])", "Test simple reverse", BOOL, true);
}

static void test_runtime_bif_cat_valid(
        struct TestContext *tc,
        struct Runtime *rt)
{
    VAL_LOC_T results[6];

    char *test_name = "BIF cat valid";
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

static void test_runtime_bif_slice(
        struct TestContext *tc,
        struct Runtime *rt)
{
    test_eval_source_expect(tc, rt, "(= (slice [ 1 2 3 ] 1 3) [ 2 3 ])", "Simple slice", BOOL, true);
    test_eval_source_expect(tc, rt, "(= (slice [ 1 2 3 ] 2 2) [])", "Empty slice", BOOL, true);
    test_eval_source_fail(tc, rt, "(slice [ 1 2 3 ] -1 0)", "Fail on negative slice index");
    test_eval_source_fail(tc, rt, "(slice [ 1 2 3 ] 1 0)", "Fail on incorrect slice indices order");
    test_eval_source_fail(tc, rt, "(slice [ 1 2 3 ] 1 4)", "Fail on slice index out of bounds");
}

static void test_runtime_bif_format(
        struct TestContext *tc,
        struct Runtime *rt)
{
    test_eval_source_fail(tc, rt, "(format \"%d%f\" { 1 })", "Format wit not enough args");
    test_eval_source_fail(tc, rt, "(format \"%d\" { 1.0 })", "Format with incorrect arg type");
    test_eval_source_fail(tc, rt, "(format \"%d\" { 1 1.0 })", "Format with too many args");

    test_eval_source_expect_string(tc, rt,
        "(format \"%s, %s!\\n\" { \"Hello\" \"World\" } )",
        "Format with two strings",
        "Hello, World!\n");

    test_eval_source_expect_string(tc, rt,
        "(format \"%d%d\" { 1 2 } )",
        "Format with consecutive wildcards",
        "12");
}

static void test_runtime_bif_parse_any(
        struct TestContext *tc,
        struct Runtime *rt)
{
    test_eval_source_fail(tc, rt, "(parse 1)", "Parse nonstring argument");

    test_eval_source_succeed(tc, rt, "(parse \"false\")", "Parse boolean value");
    test_eval_source_succeed(tc, rt, "(parse \"1\")", "Parse integer value");
    test_eval_source_succeed(tc, rt, "(parse \"2.0\")", "Parse real value");
    test_eval_source_succeed(tc, rt, "(parse \"'3'\")", "Parse character value");
    test_eval_source_succeed(tc, rt, "(parse \"\\\"four\\\"\")", "Parse string value");
    test_eval_source_succeed(tc, rt, "(parse \"{ 1 2.0 '3' [ \\\"four\\\" ]}\")", "Parse tuple successfully");
    test_eval_source_succeed(tc, rt, "(parse \"[ { 1 2 } { 3 4 } ]\")", "Parse array successfully");

    test_eval_source_expect(tc, rt,
            "(do (bind { x _ } (parse \"2.0\")) x)",
            "Parse real expect success",
            BOOL, true);

    test_eval_source_expect(tc, rt,
            "(do (bind { _ x } (parse \"2.0\")) x)",
            "Parse real expect value",
            REAL, 2.0);

    test_eval_source_expect(tc, rt,
            "(do (bind { x _ } (parse \"[ { 1 2 } { 3 } ]\")) x)",
            "Parse array homo fail",
            BOOL, false);

    test_eval_source_expect(tc, rt,
            "(do (bind { x _ } (parse \"[ 1 2 asdf ]\")) x)",
            "Parse array i-th element fail",
            BOOL, false);
}

void test_runtime_bif(struct TestContext *tc)
{
    struct Runtime *rt = rt_make(64 * 1024);

    test_runtime_bif_arythmetic(tc, rt);
    test_runtime_bif_logic(tc, rt);
    test_runtime_bif_compare(tc, rt);
    test_runtime_bif_length(tc, rt);
    test_runtime_bif_at(tc, rt);
    test_runtime_bif_reverse(tc, rt);
    test_runtime_bif_cat_valid(tc, rt);
    test_runtime_bif_cat_invalid(tc, rt);
    test_runtime_bif_slice(tc, rt);
    test_runtime_bif_format(tc, rt);
    test_runtime_bif_parse_any(tc, rt);

    rt_free(rt);
}

