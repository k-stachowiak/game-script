/* Copyright (C) 2015 Krzysztof Stachowiak */

#include <inttypes.h>

#include "error.h"

#include "parse.h"
#include "rt_val.h"

#include "test_detail.h"

bool test_runtime_sanity(struct Runtime *rt)
{
    VAL_LOC_T results[1];
	char *source = "(bind x \"x\")";
    return test_source_eval(rt, source, results);
}

bool test_runtime_free_on_fail(struct Runtime *rt)
{
    char *source = "(bind x (invalid-call))";
    struct AstNode *ast_list = parse_source(source);

    rt_consume_one(rt, ast_list, NULL, NULL);

    if (err_state()) {
        return true;
    } else {
        printf("No error on invalid ast consume.\n");
        return false;
    }
}

bool test_array_homo(struct Runtime *rt)
{
    VAL_LOC_T results[1];

    char *source_homo = "[ 1 2 3 ]";
    char *source_hetero = "[1 2.0 \"three\"]";

    if (!test_source_eval(rt, source_homo, results)) {
        printf("Failed evaluatig homogenous array.\n");
        return false;
    }

    if (test_source_eval(rt, source_hetero, results)) {
        printf("Error: Succeeded evaluating heterogenous array.\n");
        return false;
    }

    return true;
}

bool test_bif_cons_homo(struct Runtime *rt)
{
    char *source_good_1 = "(cons 'a' [])";
    char *source_good_2 = "(cons 'a' [ 'b' ])";
    char *source_bad_1 = "[ 'a' 1 ]";
    char *source_bad_2 = "(cons 'a' [ 1 ])";

    VAL_LOC_T results[1];

    if (!test_source_eval(rt, source_good_1, results)) {
        printf("Failed evaluating construction from value and empty list.\n");
        return false;
    }
    if (!test_source_eval(rt, source_good_2, results)) {
        printf("Failed evaluating construction from homogenous values.\n");
        return false;
    }

    if (test_source_eval(rt, source_bad_1, results)) {
        printf("Failed rejecting evaluation of literal of heterogenous values.\n");
        return false;
    }

    if (test_source_eval(rt, source_bad_2, results)) {
        printf("Failed rejecting construction from heterogenous value and list.\n");
        return false;
    }

    return true;
}

bool test_bif_cat_homo(struct Runtime *rt)
{
    char *source_good_1 = "(cat [ 1 2 ] [])";
    char *source_good_2 = "(cat [] [ 'a' ])";
    char *source_good_3 = "(cat [ \"asd\" \"fgh\" ] [ \"123\" ])";
    char *source_good_4 = "(cat [ ] [])";
    char *source_bad = "(cat [ 1 2 ] [ 2.0 ])";

    VAL_LOC_T results[1];

    if (!test_source_eval(rt, source_good_1, results)) {
        printf("Failed concatenating non-empty list with an empty list.\n");
        return false;
    }
    if (!test_source_eval(rt, source_good_2, results)) {
        printf("Failed concatenating empty list with a non-empty list.\n");
        return false;
    }

    if (!test_source_eval(rt, source_good_3, results)) {
        printf("Failed concatenating two non-empty lists.\n");
        return false;
    }

    if (!test_source_eval(rt, source_good_4, results)) {
        printf("Failed concatenating two empty lists.\n");
        return false;
    }

    if (test_source_eval(rt, source_bad, results)) {
        printf("Failed rejecting concatetation of heterogenous lists.\n");
        return false;
    }

    return true;
}

bool test_local_scope(struct Runtime *rt)
{
	VAL_LOC_T results[4];
	char *source =
		"(bind sqr (func (x) (* x x)))\n"
		"(bind select (func (cond x y) (if cond (sqr x) (sqr y))))\n"
		"(bind a (select true 2 3))\n"
		"(bind b (select false 2 3))\n";

	if (!test_source_eval(rt, source, results)) {
		return false;
	}

	if (rt_val_peek_int(rt, results[2]) != 4) {
		printf("Incorrect test result.\n");
		return false;
	}

	if (rt_val_peek_int(rt, results[3]) != 9) {
		printf("Incorrect test result.\n");
		return false;
	}

	return true;
}

bool test_simple_algorithm(struct Runtime *rt)
{
	VAL_LOC_T results[4];
	char *source =
		"(bind gcd (func (x y) (if (= y 0) x (gcd y (% x y)) ) ))\n"
		"(bind lcm (func (x y) (* (/ x (gcd x y)) y ) ))\n"
		"(bind x (gcd 54 24)) # Expected to be 6\n"
		"(bind y (lcm 21 6)) # Expected to be 42";

	if (!test_source_eval(rt, source, results)) {
        return false;
	}

	if (rt_val_peek_int(rt, results[2]) != 6) {
		printf("gcd returned %" PRId64 ", instead of 6.\n", rt_val_peek_int(rt, results[0]));
        return false;
	}

	if (rt_val_peek_int(rt, results[3]) != 42) {
		printf("lcm returned %" PRId64 ", instead of 42.\n", rt_val_peek_int(rt, results[1]));
        return false;
	}

	return true;
}

bool test_array_lookup(struct Runtime *rt)
{
	VAL_LOC_T results[3];
	char *source =
		"(bind min-element` (func (element array)\n"
		"    (if (empty array)\n"
		"		element\n"
        "        (if (< element (car array))\n"
        "            (min-element` element (cdr array))\n"
    	"			(min-element` (car array) (cdr array)) ) ) ))\n"
		"(bind min-element (func (array)\n"
	    "    (if (= (length array) 1)\n"
		"	    (car array)\n"
		"		(min-element` (car array) (cdr array)) ) ))\n"
		"(bind x (min-element [ 3 1 2 ]))";


	if (!test_source_eval(rt, source, results)) {
        return false;
	}

	if (!rt_val_peek_int(rt, results[2]) == 1) {
		printf("min-element found %" PRId64 ", instead of 1.\n",
                rt_val_peek_int(rt, results[0]));
        return false;
	}

	return true;
}

bool test_function_object(struct Runtime *rt)
{
	VAL_LOC_T results[3];

	char *source =
		"(bind make_doubler (func () (do\n"
		"	(bind lhs 2)\n"
		"	(func (rhs) (* lhs rhs) )\n"
		")))\n"
		"\n"
		"(bind doubler (make_doubler))\n"
		"\n"
		"(bind result (doubler 4))";

	if (!test_source_eval(rt, source, results)) {
		return false;
	}

	if (!rt_val_peek_int(rt, results[2]) == 8) {
		printf("Tricky double = %" PRId64 ", instead of 8.\n",
				rt_val_peek_int(rt, results[0]));
		return false;
	}

	return true;
}

