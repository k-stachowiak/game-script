/* Copyright (C) 2015 Krzysztof Stachowiak */

#ifndef PATTERN_H
#define PATTERN_H

enum PatternType {
	PATTERN_SYMBOL,
	PATTERN_ARRAY,
	PATTERN_TUPLE
};

struct Pattern {
	enum PatternType type;
	char *symbol;
	struct Pattern *children;
	struct Pattern *next;
};

struct Pattern *pattern_make_symbol(char *symbol);
void pattern_free(struct Pattern *pattern);

#endif
