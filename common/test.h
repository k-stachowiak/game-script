/* Copyright (C) 2015 Krzysztof Stachowiak */

#ifndef TEST_H
#define TEST_H

#include <stdbool.h>

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

#endif