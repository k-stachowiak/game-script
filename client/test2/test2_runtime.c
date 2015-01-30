#include "error.h"
#include "define.h"

#include "parse.h"
#include "runtime.h"
#include "rt_val.h"

#include "test2_detail.h"

static bool test_runtime_source_eval(
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

#define test_runtime_eval_literal(TC, RT, SRC, NAME, EX_VTYPE, EX_CTYPE, EX_VALUE) \
    do { \
        VAL_LOC_T result[1]; \
        if (!test_runtime_source_eval(RT, SRC, result)) { \
            tc_record(TC, NAME, false); \
            rt_reset(RT); \
            break; \
        } \
        if (*((VAL_HEAD_TYPE_T*)(RT->stack->buffer + *result)) != EX_VTYPE) { \
            tc_record(TC, NAME, false); \
            rt_reset(RT); \
            break; \
        } \
        if (*((EX_CTYPE*)(RT->stack->buffer + *result + VAL_HEAD_BYTES)) != EX_VALUE) { \
            tc_record(TC, NAME, false); \
            rt_reset(RT); \
            break; \
        } \
        tc_record(TC, NAME, true); \
        rt_reset(RT); \
    } while(0)

static void test_runtime_eval_literal_string(
        struct TestContext *tc,
        struct Runtime *rt,
        char *source,
        char *test_name,
        char *expected_value)
{
    VAL_LOC_T result[1];
    if (!test_runtime_source_eval(rt, source, result)) {
        tc_record(tc, test_name, false);
        rt_reset(rt);
        return;
    }
    if (*((VAL_HEAD_TYPE_T*)(rt->stack->buffer + *result)) != VAL_STRING) {
        tc_record(tc, test_name, false);
        rt_reset(rt);
        return;
    }
    if (strcmp(rt->stack->buffer + *result + VAL_HEAD_BYTES, expected_value) != 0) {
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
    test_runtime_eval_literal(tc, rt, "true", "Evaluate true literal", VAL_BOOL, VAL_BOOL_T, true);
    test_runtime_eval_literal(tc, rt, "false", "Evaluate false literal", VAL_BOOL, VAL_BOOL_T, false);
    test_runtime_eval_literal(tc, rt, "'a'", "Evaluate 'a' literal", VAL_CHAR, VAL_CHAR_T, 'a');
    test_runtime_eval_literal(tc, rt, "'\\0'", "Evaluate '\\0' literal", VAL_CHAR, VAL_CHAR_T, '\0');
    test_runtime_eval_literal(tc, rt, "1", "Evaluate 1 literal", VAL_INT, VAL_INT_T, 1);
    test_runtime_eval_literal(tc, rt, "+2", "Evaluate +2 literal", VAL_INT, VAL_INT_T, 2);
    test_runtime_eval_literal(tc, rt, "-3", "Evaluate -3 literal", VAL_INT, VAL_INT_T, -3);
    test_runtime_eval_literal(tc, rt, "1.0", "Evaluate 1.0 literal", VAL_REAL, VAL_REAL_T, 1.0);
    test_runtime_eval_literal(tc, rt, "+2.0", "Evaluate +2.0 literal", VAL_REAL, VAL_REAL_T, 2.0);
    test_runtime_eval_literal(tc, rt, "-3.0", "Evaluate -3.0 literal", VAL_REAL, VAL_REAL_T, -3.0);
    test_runtime_eval_literal_string(tc, rt, "\"\"", "Evaluate empty string literal", "");
    test_runtime_eval_literal_string(tc, rt, "\"str\"", "Evaluate simple string literal", "str");
    test_runtime_eval_literal_string(tc, rt, "\"x\\\"y\"", "Evaluate string literal with escapes", "x\\\"y");
}

static void test_runtime_eval_do_fail(struct TestContext *tc, struct Runtime *rt)
{
    char *test_name = "Fail on evaluating empty do block";
    VAL_LOC_T result[1];
    if (test_runtime_source_eval(rt, "(do)", result)) {
        tc_record(tc, test_name, false);
    } else {
        tc_record(tc, test_name, true);
    }
    rt_reset(rt);
}

static void test_runtime_eval_do_succeed(struct TestContext *tc, struct Runtime *rt)
{
    char *test_name = "Succeed on evaluating correct do block";

    VAL_LOC_T result[1];

    if (!test_runtime_source_eval(rt, "(do 1 2 3)", result)) {
        tc_record(tc, test_name, false);
        rt_reset(rt);
        return;
    }

    if (rt_val_peek_type(rt, *result) != VAL_INT || rt_val_peek_int(rt, *result) != 3) {
        tc_record(tc, test_name, false);
        rt_reset(rt);
        return;
    }

    tc_record(tc, test_name, true);
    rt_reset(rt);
}

void test2_runtime(struct TestContext *tc)
{
	struct Runtime *rt = rt_make(64 * 1024);
    test_runtime_eval_literals(tc, rt);
    test_runtime_eval_do_fail(tc, rt);
    test_runtime_eval_do_succeed(tc, rt);
    rt_free(rt);
}

