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
    rt_reset(rt);
}

static void test_runtime_bif_compare(
        struct TestContext *tc,
        struct Runtime *rt)
{
    test_eval_source_expect(tc, rt, "(eq 2 2)", "Evaluate equality true", BOOL, true);
    test_eval_source_expect(tc, rt, "(eq 12.0 23.0)", "Evaluate equality false", BOOL, false);
    test_eval_source_expect(tc, rt, "(lt 12.0 23.0)", "Evaluate less than true", BOOL, true);
    test_eval_source_expect(tc, rt, "(lt 2 2)", "Evaluate less than false", BOOL, false);

    /* Handle a curious case of parsing "+" or "-" as a numeric literal. */
    /* TODO: Put this test in the parser testing section */
    test_eval_source_fail(tc, rt, "(lt 2 -)", "Fail on performing numerical operation on a - sign");
    test_eval_source_fail(tc, rt, "(lt + 3.0)", "Fail on performing numerical operation on a + sign");

    rt_reset(rt);
}

static void test_runtime_bif_logic(
        struct TestContext *tc,
        struct Runtime *rt)
{
    test_eval_source_expect(tc, rt, "(xor false true)", "Evaluate xor true result", BOOL, true);
    test_eval_source_expect(tc, rt, "(xor true true)", "Evaluate xor false result", BOOL, false);
    test_eval_source_expect(tc, rt, "(not false)", "Evaluate not false result", BOOL, true);
    test_eval_source_expect(tc, rt, "(not true)", "Evaluate not true result", BOOL, false);
    rt_reset(rt);
}

static void test_runtime_bif_pushfb(
        struct TestContext *tc,
        struct Runtime *rt)
{
    test_eval_source_expect(tc, rt, "(eq (push_front [] 3) [ 3 ])", "Push_front to empty", BOOL, true);
    test_eval_source_expect(tc, rt, "(eq (push_front {2 3} 1) {1 2 3})", "Push_front to non-empty", BOOL, true);
    test_eval_source_expect(tc, rt, "(eq (push_back {} 3) { 3 })", "Push_back to empty", BOOL, true);
    test_eval_source_expect(tc, rt, "(eq (push_back [2 3] 1) [2 3 1])", "Push_back to non-empty", BOOL, true);
    test_eval_source_expect(tc, rt, "(eq (push_back \"\" 'a') \"a\")", "Push_back to empty string", BOOL, true);
    test_eval_source_expect(tc, rt, "(eq (push_back \"as\" 'd') \"asd\")", "Push_back to non-empty string", BOOL, true);
    rt_reset(rt);
}

static void test_runtime_bif_cat(
        struct TestContext *tc,
        struct Runtime *rt)
{
    test_eval_source_expect(tc, rt, "(eq (cat [] []) [])", "Concatenate empty arrays", BOOL, true);
    test_eval_source_expect(tc, rt, "(eq (cat {} {}) {})", "Concatenate empty tuples", BOOL, true);
    test_eval_source_expect(tc, rt, "(eq (cat \"\" \"\") \"\")", "Concatenate empty strings", BOOL, true);
    test_eval_source_expect(tc, rt, "(eq (cat [\"\"] []) [\"\"])", "Concatenate empty with non-empty array", BOOL, true);
    test_eval_source_expect(tc, rt, "(eq (cat {\"\"} {}) {\"\"})", "Concatenate empty with non-empty tuple", BOOL, true);
    test_eval_source_expect(tc, rt, "(eq (cat \"asdf\" \"\") \"asdf\")", "Concatenate empty with non-empty string", BOOL, true);

    test_eval_source_expect(tc, rt,
        "(eq (cat ['a'] ['b' 'c']) ['a' 'b' 'c'])",
        "Concatenate non-empty arrays",
        BOOL, true);

    test_eval_source_expect(tc, rt,
        "(eq (cat {1} {2.0 \"three\"}) {1 2.0 \"three\"})",
        "Concatenate non-empty tuples",
        BOOL, true);

    test_eval_source_expect(tc, rt,
        "(eq (cat \"a\" \"sd\") \"asd\")",
        "Concatenate non-empty string",
        BOOL, true);

    test_eval_source_fail(tc, rt, "(cat 1 2)", "BIF cat two non-array args");
    test_eval_source_fail(tc, rt, "(cat [ 1 ] 2)", "BIF cat one no-array args");
    test_eval_source_fail(tc, rt, "(cat 1 [ 2 ])", "BIF cat one no-array args");
    test_eval_source_fail(tc, rt, "(cat [ 1.0 ] [ 2 ])", "BIF cat hetero args");
    test_eval_source_fail(tc, rt, "(cat [ 1 ] { 2 })", "BIF cat array and tuple");
    test_eval_source_fail(tc, rt, "(cat { 1 } [ 2 ])", "BIF cat tuple and array");
    test_eval_source_fail(tc, rt, "(cat [ 1 ] { 2.0 })", "BIF cat array and tuple diffrent content types");

    rt_reset(rt);
}

static void test_runtime_bif_length(
        struct TestContext *tc,
        struct Runtime *rt)
{
    test_eval_source_expect(tc, rt, "(length [])", "Evaluate length of an empty array", INT, 0);
    test_eval_source_expect(tc, rt, "(length [ 'a' 'b' 'c' ])", "Evaluate length of an non-empty array", INT, 3);
    test_eval_source_expect(tc, rt, "(length {})", "Evaluate length of an empty tuple", INT, 0);
    test_eval_source_expect(tc, rt, "(length { 1 2.0 \"three\" })", "Evaluate length of an non-empty tuple", INT, 3);
    test_eval_source_expect(tc, rt, "(length \"\")", "Evaluate length of an empty string", INT, 0);
    test_eval_source_expect(tc, rt, "(length \"asd\\n\")", "Evaluate length of a non-empty string", INT, 4);
    test_eval_source_fail(tc, rt, "(length 'x')", "Fail on length of a non-compound value");
    rt_reset(rt);
}

static void test_runtime_bif_at(
        struct TestContext *tc,
        struct Runtime *rt)
{
    test_eval_source_expect(tc, rt, "(at [ 1 2 3 ] 0)", "Evaluate valid at call for array", INT, 1);
    test_eval_source_expect(tc, rt, "(at { 1 2.0 \"three\" } 1)", "Evaluate valid at call for tuple", REAL, 2.0);
    test_eval_source_expect(tc, rt, "(at \"asd\" 1)", "Evaluate valid at call for string", CHAR, 's');
    test_eval_source_fail(tc, rt, "(at [ 1 ] 0 0)", "Fail at on too many arguments");
    test_eval_source_fail(tc, rt, "(at {} 1.0)", "Fail at on non-integer index");
    test_eval_source_fail(tc, rt, "(at 1.0 1)", "Fail at on non-array subject");
    test_eval_source_fail(tc, rt, "(at [ \"alpha\" \"beta\" \"gamma\" ] -1)", "Fail at on negative index");
    test_eval_source_fail(tc, rt, "(at { \"alpha\" \"beta\" \"gamma\" } 3)", "Fail at on index == size");
    test_eval_source_fail(tc, rt, "(at [ \"alpha\" \"beta\" \"gamma\" ] 300)", "Fail at on index beyond size");
    rt_reset(rt);
}

static void test_runtime_bif_slice(
        struct TestContext *tc,
        struct Runtime *rt)
{
    test_eval_source_expect(tc, rt, "(eq (slice [ 1 2 3 ] 1 3) [ 2 3 ])", "Simple array slice", BOOL, true);
    test_eval_source_expect(tc, rt, "(eq (slice [ 1 2 3 ] 2 2) [])", "Empty array slice", BOOL, true);
    test_eval_source_expect(tc, rt, "(eq (slice {1 2.0 \"three\"} 1 3) {2.0 \"three\"})", "Simple tuple slice", BOOL, true);
    test_eval_source_expect(tc, rt, "(eq (slice \"asdf\" 1 3) \"sd\")", "Simple string slice", BOOL, true);
    test_eval_source_expect(tc, rt, "(eq (slice {1 2.0 \"three\"} 2 2) {})", "Empty tuple slice", BOOL, true);
    test_eval_source_fail(tc, rt, "(slice [ 1 2 3 ] -1 0)", "Fail on negative slice index");
    test_eval_source_fail(tc, rt, "(slice { 1 2 3 } 1 0)", "Fail on incorrect slice indices order");
    test_eval_source_fail(tc, rt, "(slice [ 1 2 3 ] 1 4)", "Fail on slice index out of bounds");
    rt_reset(rt);
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
    rt_reset(rt);
}

static void test_runtime_bif_parse_serialize(
        struct TestContext *tc,
        struct Runtime *rt)
{
    size_t i;
    char *valid_expressions[] = {
	"false",
	"1",
	"2.0",
	"'3'",
	"{ 1 2.0 [ '3' ] }",
	"[ { 1 2 } { 3 4 } ]"
    };

    test_eval_source_fail(tc, rt, "(parse 1)", "Parse nonstring argument");

    for (i = 0; i < sizeof(valid_expressions)/sizeof(*valid_expressions); ++i) {
	char *source = NULL;
	char *message = NULL;
	str_append(source, "(parse \"%s\")", valid_expressions[i]);
	str_append(message, "Parsing value: \"%s\"", valid_expressions[i]);
	test_eval_source_succeed(tc, rt, source, message);
	mem_free(message);
	mem_free(source);
    }

    for (i = 0; i < sizeof(valid_expressions)/sizeof(*valid_expressions); ++i) {
	char *source = NULL;
	char *message = NULL;
	str_append(
	    source,
	    "(eq %s (parse (to_string %s)))",
	    valid_expressions[i],
	    valid_expressions[i]);
	str_append(
	    message,
	    "Testing parse/serialize: %s",
	    valid_expressions[i]);
	test_eval_source_succeed(tc, rt, source, message);
	mem_free(message);
	mem_free(source);
    }

    test_eval_source_expect(tc, rt,
            "(do (bind { x _ } (parse \"2.0\")) x)",
            "Parse real expect success",
            BOOL, true);

    test_eval_source_expect(tc, rt,
            "(do (bind { _ y } (parse \"2.0\")) y)",
            "Parse real expect value",
            REAL, 2.0);

    test_eval_source_expect(tc, rt,
            "(do (bind { z _ } (parse \"[ { 1 2 } { 3 } ]\")) z)",
            "Parse array homo fail",
            BOOL, false);

    test_eval_source_expect(tc, rt,
            "(do (bind { s _ } (parse \"[ 1 2 asdf ]\")) s)",
            "Parse array i-th element fail",
            BOOL, false);

    rt_reset(rt);
}

void test_runtime_bif(struct TestContext *tc)
{
    struct Runtime *rt = rt_make();
    test_runtime_bif_arythmetic(tc, rt);
    test_runtime_bif_compare(tc, rt);
    test_runtime_bif_logic(tc, rt);
    test_runtime_bif_pushfb(tc, rt);
    test_runtime_bif_cat(tc, rt);
    test_runtime_bif_length(tc, rt);
    test_runtime_bif_at(tc, rt);
    test_runtime_bif_slice(tc, rt);
    test_runtime_bif_format(tc, rt);
    test_runtime_bif_parse_serialize(tc, rt);
    // TYPE
    rt_free(rt);
}

