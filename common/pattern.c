/* Copyright (C) 2015-2016 Krzysztof Stachowiak */

#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "memory.h"
#include "pattern.h"

struct Pattern *pattern_make_dont_care(void)
{
    struct Pattern *result = mem_malloc(sizeof(*result));

    result->type = PATTERN_DONT_CARE;
    result->next = NULL;

    return result;
}

struct Pattern *pattern_make_symbol(char *symbol)
{
    int len = strlen(symbol);
    char *copy = mem_malloc(len + 1);
    memcpy(copy, symbol, len + 1);

    struct Pattern *result = mem_malloc(sizeof(*result));

    result->type = PATTERN_SYMBOL;
    result->next = NULL;

    result->data.symbol.symbol = copy;

    return result;
}

struct Pattern *pattern_make_literal_atom_unit(void)
{
    struct Pattern *result = mem_malloc(sizeof(*result));

    result->type = PATTERN_LITERAL_ATOM;
    result->next = NULL;

    result->data.literal_atom.type = PATTERN_LITERAL_ATOM_UNIT;

    return result;
}

struct Pattern *pattern_make_literal_atom_bool(int value)
{
    struct Pattern *result = mem_malloc(sizeof(*result));

    result->type = PATTERN_LITERAL_ATOM;
    result->next = NULL;

    result->data.literal_atom.type = PATTERN_LITERAL_ATOM_BOOL;
    result->data.literal_atom.data.boolean = value;

    return result;
}

struct Pattern *pattern_make_literal_atom_int(long value)
{
    struct Pattern *result = mem_malloc(sizeof(*result));

    result->type = PATTERN_LITERAL_ATOM;
    result->next = NULL;

    result->data.literal_atom.type = PATTERN_LITERAL_ATOM_INT;
    result->data.literal_atom.data.integer = value;

    return result;
}

struct Pattern *pattern_make_literal_atom_real(double value)
{
    struct Pattern *result = mem_malloc(sizeof(*result));

    result->type = PATTERN_LITERAL_ATOM;
    result->next = NULL;

    result->data.literal_atom.type = PATTERN_LITERAL_ATOM_REAL;
    result->data.literal_atom.data.real = value;

    return result;
}

struct Pattern *pattern_make_literal_atom_character(char value)
{
    struct Pattern *result = mem_malloc(sizeof(*result));

    result->type = PATTERN_LITERAL_ATOM;
    result->next = NULL;

    result->data.literal_atom.type = PATTERN_LITERAL_ATOM_CHAR;
    result->data.literal_atom.data.character = value;

    return result;
}

struct Pattern *pattern_make_literal_atom_string(char *value)
{
    int len = strlen(value);
    char *copy = mem_malloc(len + 1);
    memcpy(copy, value, len + 1);

    struct Pattern *result = mem_malloc(sizeof(*result));

    result->type = PATTERN_LITERAL_ATOM;
    result->next = NULL;

    result->data.literal_atom.type = PATTERN_LITERAL_ATOM_STRING;
    result->data.literal_atom.data.string = copy;

    return result;
}

struct Pattern *pattern_make_literal_cpd_array(struct Pattern *children)
{
    struct Pattern *result = mem_malloc(sizeof(*result));

    result->type = PATTERN_LITERAL_COMPOUND;
    result->next = NULL;

    result->data.literal_compound.type = PATTERN_LITERAL_CPD_ARRAY;
    result->data.literal_compound.children = children;

    return result;
}

struct Pattern *pattern_make_literal_cpd_tuple(struct Pattern *children)
{
    struct Pattern *result = mem_malloc(sizeof(*result));

    result->type = PATTERN_LITERAL_COMPOUND;
    result->next = NULL;

    result->data.literal_compound.type = PATTERN_LITERAL_CPD_TUPLE;
    result->data.literal_compound.children = children;

    return result;
}

struct Pattern *pattern_make_datatype_unit(void)
{
    struct Pattern *result = mem_malloc(sizeof(*result));

    result->type = PATTERN_DATATYPE;
    result->next = NULL;

    result->data.datatype.type = PATTERN_DATATYPE_UNIT;
    result->data.datatype.children = NULL;

    return result;
}

struct Pattern *pattern_make_datatype_bool(void)
{
    struct Pattern *result = mem_malloc(sizeof(*result));

    result->type = PATTERN_DATATYPE;
    result->next = NULL;

    result->data.datatype.type = PATTERN_DATATYPE_BOOLEAN;
    result->data.datatype.children = NULL;

    return result;
}

struct Pattern *pattern_make_datatype_int(void)
{
    struct Pattern *result = mem_malloc(sizeof(*result));

    result->type = PATTERN_DATATYPE;
    result->next = NULL;

    result->data.datatype.type = PATTERN_DATATYPE_INTEGER;
    result->data.datatype.children = NULL;

    return result;
}

struct Pattern *pattern_make_datatype_real(void)
{
    struct Pattern *result = mem_malloc(sizeof(*result));

    result->type = PATTERN_DATATYPE;
    result->next = NULL;

    result->data.datatype.type = PATTERN_DATATYPE_REAL;
    result->data.datatype.children = NULL;

    return result;
}

struct Pattern *pattern_make_datatype_character(void)
{
    struct Pattern *result = mem_malloc(sizeof(*result));

    result->type = PATTERN_DATATYPE;
    result->next = NULL;

    result->data.datatype.type = PATTERN_DATATYPE_CHARACTER;
    result->data.datatype.children = NULL;

    return result;
}

struct Pattern *pattern_make_datatype_array_of(struct Pattern *child)
{
    struct Pattern *result = mem_malloc(sizeof(*result));

    result->type = PATTERN_DATATYPE;
    result->next = NULL;

    result->data.datatype.type = PATTERN_DATATYPE_ARRAY_OF;
    result->data.datatype.children = child;

    return result;
}

struct Pattern *pattern_make_datatype_reference_to(struct Pattern *child)
{
    struct Pattern *result = mem_malloc(sizeof(*result));

    result->type = PATTERN_DATATYPE;
    result->next = NULL;

    result->data.datatype.type = PATTERN_DATATYPE_REFERENCE_TO;
    result->data.datatype.children = child;

    return result;
}

struct Pattern *pattern_make_datatype_function(struct Pattern *children)
{
    struct Pattern *result = mem_malloc(sizeof(*result));

    result->type = PATTERN_DATATYPE;
    result->next = NULL;

    result->data.datatype.type = PATTERN_DATATYPE_FUNCTION;
    result->data.datatype.children = children;

    return result;
}

void pattern_free(struct Pattern *pattern)
{
    while (pattern) {
        struct Pattern *next_pattern = pattern->next;

        switch (pattern->type) {
        case PATTERN_DONT_CARE:
            break;

        case PATTERN_SYMBOL:
            mem_free(pattern->data.symbol.symbol);
            break;

        case PATTERN_LITERAL_ATOM:
            if (pattern->data.literal_atom.type == PATTERN_LITERAL_ATOM_STRING) {
                mem_free(pattern->data.literal_atom.data.string);
            }
            break;

        case PATTERN_LITERAL_COMPOUND:
            pattern_free(pattern->data.literal_compound.children);
            break;

        case PATTERN_DATATYPE:
            switch (pattern->data.datatype.type) {
                case PATTERN_DATATYPE_UNIT:
                case PATTERN_DATATYPE_BOOLEAN:
                case PATTERN_DATATYPE_INTEGER:
                case PATTERN_DATATYPE_REAL:
                case PATTERN_DATATYPE_CHARACTER:
                    break;

                case PATTERN_DATATYPE_ARRAY_OF:
                case PATTERN_DATATYPE_REFERENCE_TO:
                case PATTERN_DATATYPE_FUNCTION:
                    pattern_free(pattern->data.datatype.children);
                    break;
            }
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

bool pattern_list_contains_symbol(struct Pattern *pattern, char *symbol)
{
    while (pattern) {
        switch (pattern->type) {
        case PATTERN_SYMBOL:
            return strcmp(pattern->data.symbol.symbol, symbol) == 0;

        case PATTERN_DONT_CARE:
        case PATTERN_LITERAL_ATOM:
        case PATTERN_DATATYPE:
            break;

        case PATTERN_LITERAL_COMPOUND:
            if (pattern_list_contains_symbol(
                    pattern->data.datatype.children,
                    symbol)) {
                return true;
            }
            break;

        }
        pattern = pattern->next;
    }
    return false;
}

