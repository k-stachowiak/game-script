#include "error.h"
#include "define.h"

#include "parse.h"
#include "runtime.h"
#include "rt_val.h"

#include "test2_detail.h"

static bool test_runtime_eval_source(
        struct Runtime *rt,
        char *source,
        VAL_LOC_T *locs)
{
	int i = 0;
	struct AstNode *ast_list = NULL, *next;

	err_reset();

	ast_list = parse_source(source);
	if (!ast_list) {
		return false;
	}

	while (ast_list) {
		rt_consume_one(rt, ast_list, locs + (i++), &next);
		if (err_state()) {
			ast_node_free(next);
			return false;
		}
		ast_list = next;
	}

	return true;
}

static void test_runtime_eval_source_fail(
        struct TestContext *tc,
        struct Runtime *rt,
        char *source,
        char *test_name)
{
    VAL_LOC_T result[10];
    if (test_runtime_eval_source(rt, source, result)) {
        tc_record(tc, test_name, false);
    } else {
        tc_record(tc, test_name, true);
    }
}

static void test_runtime_eval_source_succeed(
        struct TestContext *tc,
        struct Runtime *rt,
        char *source,
        char *test_name)
{
    VAL_LOC_T result[10];
    if (test_runtime_eval_source(rt, source, result)) {
        tc_record(tc, test_name, true);
    } else {
        tc_record(tc, test_name, false);
    }
}

#define test_runtime_eval_source_expect(TC, RT, SRC, NAME, EX_TYPE, EX_VALUE) \
    do { \
        VAL_LOC_T result[1]; \
        if (!test_runtime_eval_source(RT, SRC, result)) { \
            tc_record(TC, NAME, false); \
            rt_reset(RT); \
            break; \
        } \
        if (*((VAL_HEAD_TYPE_T*)(RT->stack->buffer + *result)) != VAL_ ## EX_TYPE) { \
            tc_record(TC, NAME, false); \
            rt_reset(RT); \
            break; \
        } \
        if (*((VAL_ ## EX_TYPE ## _T*)(RT->stack->buffer + *result + VAL_HEAD_BYTES)) != EX_VALUE) { \
            tc_record(TC, NAME, false); \
            rt_reset(RT); \
            break; \
        } \
        tc_record(TC, NAME, true); \
        rt_reset(RT); \
    } while(0)

static void test_runtime_eval_source_expect_string(
        struct TestContext *tc,
        struct Runtime *rt,
        char *source,
        char *test_name,
        char *expected_value)
{
    VAL_LOC_T result[1];
    if (!test_runtime_eval_source(rt, source, result)) {
        tc_record(tc, test_name, false);
        rt_reset(rt);
        return;
    }
    if (*((VAL_HEAD_TYPE_T*)(rt->stack->buffer + *result)) != VAL_STRING) {
        tc_record(tc, test_name, false);
        rt_reset(rt);
        return;
    }
    if (!rt_val_string_eq(rt, *result, expected_value) != 0) {
        tc_record(tc, test_name, false);
        rt_reset(rt);
        return;
    }
    tc_record(tc, test_name, true);
    rt_reset(rt);
}

static void test_runtime_eval_literals(
        struct TestContext *tc,
        struct Runtime *rt)
{
    test_runtime_eval_source_expect(tc, rt, "true", "Evaluate true literal", BOOL, true);
    test_runtime_eval_source_expect(tc, rt, "false", "Evaluate false literal", BOOL, false);
    test_runtime_eval_source_expect(tc, rt, "'a'", "Evaluate 'a' literal", CHAR, 'a');
    test_runtime_eval_source_expect(tc, rt, "'\\0'", "Evaluate '\\0' literal", CHAR, '\0');
    test_runtime_eval_source_expect(tc, rt, "1", "Evaluate 1 literal", INT, 1);
    test_runtime_eval_source_expect(tc, rt, "+2", "Evaluate +2 literal", INT, 2);
    test_runtime_eval_source_expect(tc, rt, "-3", "Evaluate -3 literal", INT, -3);
    test_runtime_eval_source_expect(tc, rt, "1.0", "Evaluate 1.0 literal", REAL, 1.0);
    test_runtime_eval_source_expect(tc, rt, "+2.0", "Evaluate +2.0 literal", REAL, 2.0);
    test_runtime_eval_source_expect(tc, rt, "-3.0", "Evaluate -3.0 literal", REAL, -3.0);
    test_runtime_eval_source_expect_string(tc, rt, "\"\"", "Evaluate empty string literal", "");
    test_runtime_eval_source_expect_string(tc, rt, "\"str\"", "Evaluate simple string literal", "str");
    test_runtime_eval_source_expect_string(tc, rt, "\"x\\\"y\"", "Evaluate string literal with escapes", "x\\\"y");
}

static void test_runtime_eval_do(struct TestContext *tc, struct Runtime *rt)
{
    test_runtime_eval_source_fail(tc, rt, "(do)", "Fail on evaluating empty do block");
    test_runtime_eval_source_expect(tc, rt, "(do 1 2 3)", "Succeed on evaluating correct do block", INT, 3);
}

static void test_runtime_eval_bind(struct TestContext *tc, struct Runtime *rt)
{
    test_runtime_eval_source_fail(tc, rt, "(bind)", "Fail on evaluating empty bind block");
    test_runtime_eval_source_fail(tc, rt, "(bind x)", "Fail on missing bind value");
    test_runtime_eval_source_fail(tc, rt, "(bind \"x\" 1)", "Fail on binding to literal");
    test_runtime_eval_source_expect(tc, rt, "(bind x 1.0)", "Succeed on simple bind", REAL, 1.0);
    test_runtime_eval_source_expect(tc, rt, "(bind y (+ 2 3))", "Succeed on non-trivial bind", INT, 5);
}

static void test_runtime_eval_iff(struct TestContext *tc, struct Runtime *rt)
{
    test_runtime_eval_source_fail(tc, rt, "(if)", "Fail on evaluating empty if block");
    test_runtime_eval_source_fail(tc, rt, "(if a)", "Fail on evaluating incomplete if block 1");
    test_runtime_eval_source_fail(tc, rt, "(if a b)", "Fail on evaluating incomplete if block 2");
    test_runtime_eval_source_expect(tc, rt, "(if true 1.0 2.0)", "Succeed on iff - true branch", REAL, 1.0);
    test_runtime_eval_source_expect_string(tc, rt, "(if false \"a\" \"b\")", "Succeed on iff - false branch", "b");
}

static void test_runtime_eval_reference(
        struct TestContext *tc,
        struct Runtime *rt)
{
    test_runtime_eval_source_fail(tc, rt, "x", "Fail on evaluating undefined reference");
    test_runtime_eval_source_expect(tc, rt, "(do (bind y 3) y)", "Succeed on evaluating defined reference", INT, 3);
}

static void test_runtime_eval_cpd(struct TestContext *tc, struct Runtime *rt)
{
    test_runtime_eval_source_fail(tc, rt, "[ 1.0 \"two\"]", "Fail on evaluating heterogenous array");
    test_runtime_eval_source_fail(tc, rt, "[ [ 1.0 ]  [ \"two\" ] ]", "Fail on evaluating heterogenous nested array");
    test_runtime_eval_source_succeed(tc, rt, "[]", "Succeed on evaluating empty array");
    test_runtime_eval_source_succeed(tc, rt, "[ 1 2 ]", "Succeed on evaluating homogenous array");
    test_runtime_eval_source_succeed(tc, rt, "[ [ 1 ] [ 2 3 ] ]", "Succeed on evaluating homogenous nested array");
    test_runtime_eval_source_succeed(tc, rt, "{}", "Succeed on evaluating empty tuple");
    test_runtime_eval_source_succeed(tc, rt, "{ 1 2 }", "Succeed on evaluating homogenous tuple");
    test_runtime_eval_source_succeed(tc, rt, "{ 1 [] }", "Succeed on evaluating heterogenous tuple");
}

void test2_runtime(struct TestContext *tc)
{
	struct Runtime *rt = rt_make(64 * 1024);
    test_runtime_eval_literals(tc, rt);
    test_runtime_eval_do(tc, rt);
    test_runtime_eval_bind(tc, rt);
    test_runtime_eval_iff(tc, rt);
    test_runtime_eval_reference(tc, rt);
    test_runtime_eval_iff(tc, rt);
    test_runtime_eval_cpd(tc, rt);
    rt_free(rt);
}

