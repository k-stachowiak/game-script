/* Copyright (C) 2015 Krzysztof Stachowiak */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "test_detail.h"

int main()
{
    struct TestContext tc;

    atexit(err_reset);

    tc_init(&tc);
    test_front(&tc);
    test_runtime_basic(&tc);
    test_runtime_func(&tc);
    test_runtime_bif(&tc);
    test_runtime_parafunc(&tc);
    test_runtime_scope(&tc);
    test_cci(&tc);
    tc_report(&tc);
    tc_deinit(&tc);

    return 0;
}
