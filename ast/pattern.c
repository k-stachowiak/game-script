/* Copyright (C) 2015 Krzysztof Stachowiak */

#include "memory.h"
#include "pattern.h"

struct Pattern *pattern_make_symbol(char *symbol)
{
	struct Pattern *result = mem_malloc(sizeof(*result));
	result->type = PATTERN_SYMBOL;
	result->symbol = symbol;
	result->children = NULL;
	result->next = NULL;
	return result;
}

void pattern_free(struct Pattern *pattern)
{
	while (pattern) {
		struct Pattern *next_pattern = pattern->next;
		switch (pattern->type) {
		case PATTERN_SYMBOL:
			mem_free(pattern->symbol);
			break;

		case PATTERN_ARRAY:
		case PATTERN_TUPLE:
			pattern_free(pattern->children);
			break;
		}

		mem_free(pattern);
		pattern = next_pattern;
	}
}

int pattern_list_len(struct Pattern *pattern)
{
    int result = 0;
    while (pattern) {
        ++result;
        pattern = pattern->next;
    }
    return result;
}
