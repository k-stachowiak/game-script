/* Copyright (C) 2015-2016 Krzysztof Stachowiak */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "test_detail.h"

int main(int argc, char **argv)
{
    struct TestContext tc;
    int result = 0;

    atexit(err_reset);

    tc_init(&tc);

    switch (argc) {
    case 1:
	test_front(&tc);
	test_runtime_basic(&tc);
	test_runtime_func(&tc);
	test_runtime_bif(&tc);
	test_runtime_parafunc(&tc);
	test_runtime_scope(&tc);
	break;
    case 2:
	if (strcmp(argv[1], "cci") == 0) {
	    test_cci(&tc);
	} else {
	    fprintf(stderr, "Incorrect test command\n");
	    result = 1; // bar x y [ ]
	    goto end;
	}
	break;

    default:
		fprintf(stderr, "Incorrect number of arguments\n");
		result = 1;
		goto end;
    }

    tc_report(&tc);
    
 end:
    tc_deinit(&tc);

    return result;
}
