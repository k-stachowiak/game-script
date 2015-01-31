/* Copyright (C) 2015 Krzysztof Stachowiak */

#include <inttypes.h>

#include "error.h"

#include "parse.h"
#include "rt_val.h"

#include "test_detail.h"

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

