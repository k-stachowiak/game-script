/* Copyright (C) 2015 Krzysztof Stachowiak */

#ifndef PATTERN_H
#define PATTERN_H

#include <stdbool.h>

enum PatternType {
    PATTERN_DONT_CARE,
    PATTERN_SYMBOL,
    PATTERN_LITERAL_ATOMIC,
    PATTERN_LITERAL_COMPOUND,
    PATTERN_DATATYPE
};

enum PatternLiteralAtomicType {
    PATTERN_LITERAL_ATOMIC_UNIT,
    PATTERN_LITERAL_ATOMIC_BOOL,
    PATTERN_LITERAL_ATOMIC_INT,
    PATTERN_LITERAL_ATOMIC_REAL,
    PATTERN_LITERAL_ATOMIC_CHAR,
    PATTERN_LITERAL_ATOMIC_STRING
};

enum PatternLiteralCompoundType {
    PATTERN_LITERAL_CPD_ARRAY,
    PATTERN_LITERAL_CPD_TUPLE
};

enum PatternDataTypeType {
    PATTERN_DATATYPE_UNIT,
    PATTERN_DATATYPE_BOOLEAN,
    PATTERN_DATATYPE_INTEGER,
    PATTERN_DATATYPE_REAL,
    PATTERN_DATATYPE_CHARACTER,
    PATTERN_DATATYPE_ARRAY_OF,
    PATTERN_DATATYPE_REFERENCE_TO,
    PATTERN_DATATYPE_FUNCTION
};

struct PatternDontCare {
    /* Structure intentionally left blank */
    char DUMMY_FIELD;
};

struct PatternSymbol {
    char *symbol;
};

struct PatternLiteralAtomic {
    enum PatternLiteralAtomicType type;
    union {
        int boolean;
        long integer;
        double real;
        char character;
        char *string;
    } data;
};

struct PatternLiteralCompound {
    enum PatternLiteralCompoundType type;
    struct Pattern *children;
};

struct PatternDataType {
    enum PatternDataTypeType type;
    struct Pattern *children;
};

struct Pattern {
    enum PatternType type;
    union {
        struct PatternDontCare dont_care;
        struct PatternSymbol symbol;
        struct PatternLiteralAtomic literal_atomic;
        struct PatternLiteralCompound literal_compound;
        struct PatternDataType datatype;
    } data;
    struct Pattern *next;
};

struct Pattern *pattern_make_dont_care(void);

struct Pattern *pattern_make_symbol(char *symbol);

struct Pattern *pattern_make_literal_atomic_unit(void);
struct Pattern *pattern_make_literal_atomic_bool(int value);
struct Pattern *pattern_make_literal_atomic_int(long value);
struct Pattern *pattern_make_literal_atomic_real(double value);
struct Pattern *pattern_make_literal_atomic_character(char value);
struct Pattern *pattern_make_literal_atomic_string(char *value);

struct Pattern *pattern_make_literal_cpd_array(struct Pattern *children);
struct Pattern *pattern_make_literal_cpd_tuple(struct Pattern *children);

struct Pattern *pattern_make_datatype_unit(void);
struct Pattern *pattern_make_datatype_bool(void);
struct Pattern *pattern_make_datatype_int(void);
struct Pattern *pattern_make_datatype_real(void);
struct Pattern *pattern_make_datatype_character(void);
struct Pattern *pattern_make_datatype_array_of(struct Pattern *child);
struct Pattern *pattern_make_datatype_reference_to(struct Pattern *child);
struct Pattern *pattern_make_datatype_function(struct Pattern *children);

void pattern_free(struct Pattern *pattern);

char *pattern_serialize(struct Pattern *pattern);

int pattern_list_len(struct Pattern *pattern);
bool pattern_list_contains_symbol(struct Pattern *pattern, char *symbol);

#endif
