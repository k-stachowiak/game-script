/* Copyright (C) 2015 Krzysztof Stachowiak */

#ifndef TEST_DETAIL_H
#define TEST_DETAIL_H

#include <stdbool.h>

#include "error.h"
#include "collection.h"

#include "lex.h"
#include "dom.h"
#include "parse.h"
#include "runtime.h"
#include "rt_val.h"
#include "test.h"

/* Main test procedures.
 * =====================
 */

void test_front(struct TestContext *tc);
void test_runtime_basic(struct TestContext *tc);
void test_runtime_func(struct TestContext *tc);
void test_runtime_bif(struct TestContext *tc);
void test_runtime_parafunc(struct TestContext *tc);
void test_runtime_scope(struct TestContext *tc);
void test_runtime_ref(struct TestContext *tc);
void test_cci(struct TestContext *tc);

#endif

