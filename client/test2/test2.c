/* Copyright (C) 2015 Krzysztof Stachowiak */

#include <stdlib.h>
#include <stdio.h>

#include "collection.h"

#include "test2_detail.h"

void tc_init(struct TestContext *tc)
{
    tc->entries.data = NULL;
    tc->entries.cap = 0;
    tc->entries.size = 0;
}

void tc_deinit(struct TestContext *tc)
{
    ARRAY_FREE(tc->entries);
}

void tc_report(struct TestContext *tc)
{
    int i, successes;
    char *success_string;
    for (i = 0; i < tc->entries.size; ++i) {

        struct TestEntry *te = tc->entries.data + i;

        if (te->result) {
            ++successes;
            success_string = "OK";
        } else {
            success_string = "FAIL";
        }

        printf("[%s] %s\n", success_string, te->name);
    }

    printf("\n%d/%d tests passed.\n", successes, i);
}

void tc_record(struct TestContext *tc, char *name, bool result)
{
    struct TestEntry te = { name, result };
    ARRAY_APPEND(tc->entries, te);
}

int test2(int argc, char *argv[])
{
    struct TestContext tc;

    tc_init(&tc);
    test2_front(&tc);
    tc_report(&tc);
    tc_deinit(&tc);

    return 0;
}

