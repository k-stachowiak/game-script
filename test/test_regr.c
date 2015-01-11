/* Copyright (C) 2015 Krzysztof Stachowiak */

#include <stdio.h>

#include "test_detail.h"

bool test_regression_real_in_array(struct Runtime *rt)
{
    VAL_LOC_T results[1];

    char *source = "[1 2.0]";

    if (test_source_eval(rt, source, results)) {
        printf("Error: succeeded evaluating heterogenous array literal.");
        return false;
    }

    return true;
}
