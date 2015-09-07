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
    result->data.symbol.symbol = symbol;
    result->next = NULL;
    return result;
}

struct Pattern *pattern_make_dontcare(void)
{
    struct Pattern *result = mem_malloc(sizeof(*result));
    result->type = PATTERN_DONTCARE;
    result->next = NULL;
    return result;
}

struct Pattern *pattern_make_array(struct Pattern *children)
{
    struct Pattern *result = mem_malloc(sizeof(*result));
    result->type = PATTERN_ARRAY;
    result->data.compound.children = children;
    result->next = NULL;
    return result;
}

struct Pattern *pattern_make_tuple(struct Pattern *children)
{
    struct Pattern *result = mem_malloc(sizeof(*result));
    result->type = PATTERN_TUPLE;
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
            mem_free(pattern->data.symbol.symbol);
            break;

        case PATTERN_DONTCARE:
            break;

        case PATTERN_ARRAY:
        case PATTERN_TUPLE:
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
            if (strcmp(pattern->data.symbol.symbol, symbol) == 0) {
                return true;
            }
            break;

        case PATTERN_DONTCARE:
            break;

        case PATTERN_ARRAY:
        case PATTERN_TUPLE:
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

