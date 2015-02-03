/* Copyright (C) 2015 Krzysztof Stachowiak */

#ifndef TEST_DETAIL_H
#define TEST_DETAIL_H

#include <stdbool.h>

#include "define.h"
#include "error.h"
#include "collection.h"

#include "lex.h"
#include "dom.h"
#include "parse.h"
#include "runtime.h"
#include "rt_val.h"

/* Test context API.
 * =================
 */

struct TestEntry {
    char *name;
    bool result;
};

struct TestContext {
    struct {
        struct TestEntry *data;
        int cap, size;
    } entries;
};

void tc_init(struct TestContext *tc);
void tc_deinit(struct TestContext *tc);
void tc_report(struct TestContext *tc);
void tc_record(struct TestContext *tc, char *name, bool result);

/* Main test procedures.
 * =====================
 */

void test_front(struct TestContext *tc);
void test_runtime_basic(struct TestContext *tc);
void test_runtime_func(struct TestContext *tc);
void test_runtime_bif(struct TestContext *tc);

#endif

