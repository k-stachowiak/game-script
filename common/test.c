#include <stdlib.h>
#include <stdio.h>

#include "collection.h"

#include "test.h"

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
	int i, successes = 0;
	for (i = 0; i < tc->entries.size; ++i) {
		struct TestEntry *te = tc->entries.data + i;
		if (te->result) {
			++successes;
		}
		else {
			printf("[FAIL] %s\n", te->name);
		}
	}

	printf("%d/%d tests passed.\n", successes, i);
}

void tc_record(struct TestContext *tc, char *name, bool result)
{
	struct TestEntry te = { name, result };
	ARRAY_APPEND(tc->entries, te);
}
