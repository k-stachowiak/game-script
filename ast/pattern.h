/* Copyright (C) 2015 Krzysztof Stachowiak */

#ifndef PATTERN_H
#define PATTERN_H

#include <stdbool.h>

enum PatternType {
    PATTERN_SYMBOL,
    PATTERN_DONTCARE,
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
struct Pattern *pattern_make_dontcare(void);
struct Pattern *pattern_make_compound(
        struct Pattern *children,
        enum PatternType type);

void pattern_free(struct Pattern *pattern);
int pattern_list_len(struct Pattern *pattern);
bool pattern_list_contains_symbol(struct Pattern *pattern, char *symbol);

#endif
