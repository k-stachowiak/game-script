/* Copyright (C) 2015 Krzysztof Stachowiak */

#include "test_helpers.h"

static void test_cci_impl(struct TestContext *tc, struct Runtime *rt)
{
	test_eval_source_succeed(tc, rt,
		"(bind empty (func (x) (eq (length x) 0)))\n"
		"(bind cons (func (x arr) (push_front arr x)))\n"
		"(bind car (func (x) (at x 0)))\n"
		"(bind cdr (func (x) (slice x 1 (length x))))\n"
		"(bind zip_with (func (f va vb)\n"
		"    (if (or (empty va) (empty vb))\n"
		"        (slice va 0 0)\n"
		"        (cons\n"
		"            (f (car va) (car vb))\n"
		"            (zip_with f (cdr va) (cdr vb))\n"
		"        )\n"
		"    )\n"
		"))\n",
		"Prepare source for CCI");
	test_eval_source_expect(tc, rt,
		"(eq (zip_with + [ 1 2 3 ] [ 2 3 4 ]) [ 2 6 12])",
		"Execute code exposing CCI",
		BOOL, true);
}

void test_cci(struct TestContext *tc)
{
    struct Runtime *rt = rt_make(64 * 1024);
    test_cci_impl(tc, rt);
    rt_free(rt);
}
