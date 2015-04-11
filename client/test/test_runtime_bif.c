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
}

static void test_runtime_bif_length(
        struct TestContext *tc,
        struct Runtime *rt)
{
    test_eval_source_expect(tc, rt, "(length [])", "Evaluate length of an empty array", INT, 0);
    test_eval_source_expect(tc, rt, "(length [ 'a' 'b' 'c' ])", "Evaluate length of an non-empty array", INT, 3);
    test_eval_source_expect(tc, rt, "(length {})", "Evaluate length of an empty tuple", INT, 0);
    test_eval_source_expect(tc, rt, "(length { 1 2.0 \"three\" })", "Evaluate length of an non-empty tuple", INT, 3);
    test_eval_source_fail(tc, rt, "(length \"asdf\")", "Fail on length of a non-compound value");
}

static void test_runtime_bif_at(
        struct TestContext *tc,
        struct Runtime *rt)
{
    test_eval_source_expect(tc, rt, "(at [ 1 2 3 ] 0)", "Evaluate valid at call for array", INT, 1);
    test_eval_source_expect(tc, rt, "(at { 1 2.0 \"three\" } 1)", "Evaluate valid at call for tuple", REAL, 2.0);
    test_eval_source_fail(tc, rt, "(at [ 1 ] 0 0)", "Fail at on too many arguments");
    test_eval_source_fail(tc, rt, "(at {} 1.0)", "Fail at on non-integer index");
    test_eval_source_fail(tc, rt, "(at 1.0 1)", "Fail at on non-array subject");
    test_eval_source_fail(tc, rt, "(at [ \"alpha\" \"beta\" \"gamma\" ] -1)", "Fail at on negative index");
    test_eval_source_fail(tc, rt, "(at { \"alpha\" \"beta\" \"gamma\" } 3)", "Fail at on index == size");
    test_eval_source_fail(tc, rt, "(at [ \"alpha\" \"beta\" \"gamma\" ] 300)", "Fail at on index beyond size");
}

static void test_runtime_bif_reverse(
        struct TestContext *tc,
        struct Runtime *rt)
{
    test_eval_source_expect(tc, rt,
        "(= (reverse [ 'b' 'c' 'a' ]) [ 'a' 'c' 'b' ])",
        "Test simple reverse",
        BOOL, true);

    test_eval_source_expect(tc, rt,
        "(= (reverse { 1 2.0 \"three\" }) { \"three\" 2.0 1 })",
        "Test simple reverse",
        BOOL, true);
}

static void test_runtime_bif_cat(
        struct TestContext *tc,
        struct Runtime *rt)
{
    test_eval_source_expect(tc, rt, "(= (cat [] []) [])", "Concatenate empty arrays", BOOL, true);
    test_eval_source_expect(tc, rt, "(= (cat {} {}) {})", "Concatenate empty tuples", BOOL, true);
    test_eval_source_expect(tc, rt, "(= (cat [\"\"] []) [\"\"])", "Concatenate empty with non-empty array", BOOL, true);
    test_eval_source_expect(tc, rt, "(= (cat {\"\"} {}) {\"\"})", "Concatenate empty with non-empty tuple", BOOL, true);

    test_eval_source_expect(tc, rt,
        "(= (cat ['a'] ['b' 'c']) ['a' 'b' 'c'])",
        "Concatenate non-empty arrays",
        BOOL, true);

    test_eval_source_expect(tc, rt,
        "(= (cat {1} {2.0 \"three\"}) {1 2.0 \"three\"})",
        "Concatenate non-empty tuples",
        BOOL, true);

    test_eval_source_fail(tc, rt, "(cat 1 2)", "BIF cat two non-array args");
    test_eval_source_fail(tc, rt, "(cat [ 1 ] 2)", "BIF cat one no-array args");
    test_eval_source_fail(tc, rt, "(cat 1 [ 2 ])", "BIF cat one no-array args");
    test_eval_source_fail(tc, rt, "(cat [ 1.0 ] [ 2 ])", "BIF cat hetero args");
    test_eval_source_fail(tc, rt, "(cat [ 1 ] { 2 })", "BIF cat array and tuple");
    test_eval_source_fail(tc, rt, "(cat { 1 } [ 2 ])", "BIF cat tuple and array");
    test_eval_source_fail(tc, rt, "(cat [ 1 ] { 2.0 })", "BIF cat array and tuple diffrent content types");
}

static void test_runtime_bif_slice(
        struct TestContext *tc,
        struct Runtime *rt)
{
    test_eval_source_expect(tc, rt, "(= (slice [ 1 2 3 ] 1 3) [ 2 3 ])", "Simple array slice", BOOL, true);
    test_eval_source_expect(tc, rt, "(= (slice [ 1 2 3 ] 2 2) [])", "Empty array slice", BOOL, true);
    test_eval_source_expect(tc, rt, "(= (slice {1 2.0 \"three\"} 1 3) {2.0 \"three\"})", "Simple tuple slice", BOOL, true);
    test_eval_source_expect(tc, rt, "(= (slice {1 2.0 \"three\"} 2 2) {})", "Empty tuple slice", BOOL, true);
    test_eval_source_fail(tc, rt, "(slice [ 1 2 3 ] -1 0)", "Fail on negative slice index");
    test_eval_source_fail(tc, rt, "(slice { 1 2 3 } 1 0)", "Fail on incorrect slice indices order");
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
    test_runtime_bif_cat(tc, rt);
    test_runtime_bif_slice(tc, rt);
    test_runtime_bif_format(tc, rt);
    test_runtime_bif_parse_any(tc, rt);

    rt_free(rt);
}

