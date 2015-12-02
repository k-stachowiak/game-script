/* Copyright (C) 2015 Krzysztof Stachowiak */

#ifndef PATTERN_H
#define PATTERN_H

#include <stdbool.h>

enum PatternType {
    PATTERN_LITERAL,
    PATTERN_CPD_ARRAY,
    PATTERN_CPD_TUPLE,
    PATTERN_SYMBOL,
    PATTERN_SYMBOL_DONTCARE
};

enum PatternLiteralType {
    PATTERN_LIT_UNIT,
    PATTERN_LIT_BOOL,
    PATTERN_LIT_STRING,
    PATTERN_LIT_CHAR,
    PATTERN_LIT_INT,
    PATTERN_LIT_REAL
};

struct PatternLiteral {
    enum PatternLiteralType type;
    union {
        int boolean;
        char *string;
        char character;
        long integer;
        double real;
    } data;
};

struct PatternCompound {
    struct Pattern *children;
};

struct PatternSymbol {
    char *symbol;
};

struct Pattern {
    enum PatternType type;
    union {
        struct PatternLiteral literal;
        struct PatternCompound compound;
        struct PatternSymbol symbol;
    } data;
    struct Pattern *next;
};

struct Pattern *pattern_make_symbol(char *symbol);
struct Pattern *pattern_make_literal_unit(void);
struct Pattern *pattern_make_literal_bool(int value);
struct Pattern *pattern_make_literal_string(char *value);
struct Pattern *pattern_make_literal_character(char value);
struct Pattern *pattern_make_literal_int(long value);
struct Pattern *pattern_make_literal_real(double value);
struct Pattern *pattern_make_dontcare(void);
struct Pattern *pattern_make_array(struct Pattern *children);
struct Pattern *pattern_make_tuple(struct Pattern *children);

void pattern_free(struct Pattern *pattern);

int pattern_list_len(struct Pattern *pattern);
bool pattern_list_contains_symbol(struct Pattern *pattern, char *symbol);

#endif
