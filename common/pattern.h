/* Copyright (C) 2015 Krzysztof Stachowiak */

#ifndef PATTERN_H
#define PATTERN_H

#include <stdbool.h>

enum TypePatternType {
    TPT_BOOL,
    TPT_INTEGER,
    TPT_REAL,
    TPT_CHARACTER,
    TPT_ARRAY,
    TPT_TUPLE
};

struct TypePattern {
    enum TypePatternType type;
    struct TypePattern *children;
    struct TypePattern *next;
};

struct TypePattern *type_pattern_make_bool(void);
struct TypePattern *type_pattern_make_integer(void);
struct TypePattern *type_pattern_make_real(void);
struct TypePattern *type_pattern_make_character(void);
struct TypePattern *type_pattern_make_array(struct TypePattern *children);
struct TypePattern *type_pattern_make_tuple(struct TypePattern *children);

void type_pattern_free(struct TypePattern *tp);
int type_pattern_list_length(struct TypePattern *tp);

enum PatternType {
    PATTERN_SYMBOL,
    PATTERN_DONTCARE,
    PATTERN_ARRAY,
    PATTERN_TUPLE,
    PATTERN_MATCHING
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

struct PatternMatching {
    struct TypePattern *type_pattern;
    struct Pattern *pattern;
};

struct Pattern {
    enum PatternType type;
    union {
        struct PatternSymbol symbol;
        struct PatternDontCare dont_care;
        struct PatternCompound compound;
        struct PatternMatching matching;
    } data;
    struct Pattern *next;
};

struct Pattern *pattern_make_symbol(char *symbol);
struct Pattern *pattern_make_dontcare(void);
struct Pattern *pattern_make_array(struct Pattern *children);
struct Pattern *pattern_make_tuple(struct Pattern *children);
struct Pattern *pattern_make_matching(
    struct TypePattern *type_pattern,
    struct Pattern *pattern);

void pattern_free(struct Pattern *pattern);

int pattern_list_len(struct Pattern *pattern);
bool pattern_list_contains_symbol(struct Pattern *pattern, char *symbol);

#endif
