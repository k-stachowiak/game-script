/* Copyright (C) 2015 Krzysztof Stachowiak */

#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "memory.h"
#include "pattern.h"

struct Pattern *pattern_make_symbol(char *symbol)
{
    struct Pattern *result = mem_malloc(sizeof(*result));
    result->type = PATTERN_SYMBOL;
    result->data.symbol.type = PATTERN_SYM_REGULAR;
    result->data.symbol.symbol = symbol;
    result->next = NULL;
    return result;
}

struct Pattern *pattern_make_dontcare(void)
{
    struct Pattern *result = mem_malloc(sizeof(*result));
    result->type = PATTERN_SYMBOL;
    result->data.symbol.type = PATTERN_SYM_DONT_CARE;
    result->data.symbol.symbol = NULL;
    result->next = NULL;
    return result;
}

struct Pattern *pattern_make_literal_unit(void)
{
    struct Pattern *result = mem_malloc(sizeof(*result));
    result->type = PATTERN_LITERAL;
    result->data.literal.type = PATTERN_LIT_UNIT;
    result->next = NULL;
    return result;
}

struct Pattern *pattern_make_literal_bool(int value)
{
    struct Pattern *result = mem_malloc(sizeof(*result));
    result->type = PATTERN_LITERAL;
    result->data.literal.type = PATTERN_LIT_BOOL;
    result->data.literal.data.boolean = value;
    result->next = NULL;
    return result;
}

struct Pattern *pattern_make_literal_string(char *value)
{
    struct Pattern *result = mem_malloc(sizeof(*result));

    int len = strlen(value);
    char *copy = mem_malloc(len + 1);
    memcpy(copy, value, len + 1);

    result->type = PATTERN_LITERAL;
    result->data.literal.type = PATTERN_LIT_STRING;
    result->data.literal.data.string = copy;
    result->next = NULL;
    return result;
}

struct Pattern *pattern_make_literal_character(char value)
{
    struct Pattern *result = mem_malloc(sizeof(*result));
    result->type = PATTERN_LITERAL;
    result->data.literal.type = PATTERN_LIT_CHAR;
    result->data.literal.data.character = value;
    result->next = NULL;
    return result;
}

struct Pattern *pattern_make_literal_int(long value)
{
    struct Pattern *result = mem_malloc(sizeof(*result));
    result->type = PATTERN_LITERAL;
    result->data.literal.type = PATTERN_LIT_INT;
    result->data.literal.data.integer = value;
    result->next = NULL;
    return result;
}

struct Pattern *pattern_make_literal_real(double value)
{
    struct Pattern *result = mem_malloc(sizeof(*result));
    result->type = PATTERN_LITERAL;
    result->data.literal.type = PATTERN_LIT_REAL;
    result->data.literal.data.real = value;
    result->next = NULL;
    return result;
}

struct Pattern *pattern_make_array(struct Pattern *children)
{
    struct Pattern *result = mem_malloc(sizeof(*result));
    result->type = PATTERN_CPD_ARRAY;
    result->data.compound.children = children;
    result->next = NULL;
    return result;
}

struct Pattern *pattern_make_tuple(struct Pattern *children)
{
    struct Pattern *result = mem_malloc(sizeof(*result));
    result->type = PATTERN_CPD_TUPLE;
    result->data.compound.children = children;
    result->next = NULL;
    return result;
}

void pattern_free(struct Pattern *pattern)
{
    while (pattern) {
        struct Pattern *next_pattern = pattern->next;

        switch (pattern->type) {
        case PATTERN_SYMBOL:
            switch (pattern->data.symbol.type) {
            case PATTERN_SYM_REGULAR:
                mem_free(pattern->data.symbol.symbol);
                break;
            case PATTERN_SYM_DONT_CARE:
                break;
            }
            break;

        case PATTERN_LITERAL:
            if (pattern->data.literal.type == PATTERN_LIT_STRING) {
                mem_free(pattern->data.literal.data.string);
            }
            break;

        case PATTERN_CPD_ARRAY:
        case PATTERN_CPD_TUPLE:
            pattern_free(pattern->data.compound.children);
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
            switch (pattern->data.symbol.type) {
            case PATTERN_SYM_REGULAR:
                if (strcmp(pattern->data.symbol.symbol, symbol) == 0) {
                    return true;
                }
                break;
            case PATTERN_SYM_DONT_CARE:
                break;
            }
            break;

        case PATTERN_LITERAL:
            break;

        case PATTERN_CPD_ARRAY:
        case PATTERN_CPD_TUPLE:
            if (pattern_list_contains_symbol(
                    pattern->data.compound.children,
                    symbol)) {
                return true;
            }
            break;

        }
        pattern = pattern->next;
    }
    return false;
}

