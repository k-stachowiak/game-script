/* Copyright (C) 2015 Krzysztof Stachowiak */

#include <stdio.h>

#include "test_detail.h"

bool test_regression_comment(struct Runtime *rt)
{
	VAL_LOC_T results[2];
	char *source =
		"#start with a comment\n"
		"(bind # end line with a comment\n"
		"    #interleave with a comment   \n"
		"x 0)\n"
		"(bind y 42) # comment at the end of the source\n";
	
	if (!test_source_eval(rt, source, results)) {
		printf("Failed parsing source code with comments.\n");
		return false;
	}

	return true;
}

bool test_regression_cyclic_calls(struct Runtime *rt)
{
    VAL_LOC_T results[2];
    char *source =
        "(bind mul-by-four (func (x)\n"
        "    (mul-by-two (mul-by-two x))\n"
        "))\n"
        "\n"
        "(bind mul-by-two (func (x)\n"
        "    (* 2 x)\n"
        "))";

    if (!test_source_eval(rt, source, results)) {
        printf("Failed parsing source with cyclic calls.\n");
        return false;
    }

    return true;
}

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

