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

struct PatternSymbol {
    char *symbol;
};

struct PatternDontCare {
    /* Intentionally left blank */
};

struct PatternCompound {
    struct Pattern *children;
};

struct Pattern {
    enum PatternType type;
    union {
        struct PatternSymbol symbol;
        struct PatternDontCare dont_care;
        struct PatternCompound compound;
    } data;
    struct Pattern *next;
};

struct Pattern *pattern_make_symbol(char *symbol);
struct Pattern *pattern_make_dontcare(void);
struct Pattern *pattern_make_array(struct Pattern *children);
struct Pattern *pattern_make_tuple(struct Pattern *children);

void pattern_free(struct Pattern *pattern);

int pattern_list_len(struct Pattern *pattern);
bool pattern_list_contains_symbol(struct Pattern *pattern, char *symbol);

#endif
