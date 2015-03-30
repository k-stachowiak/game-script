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
    result->symbol = symbol;
    result->children = NULL;
    result->next = NULL;
    return result;
}

struct Pattern *pattern_make_compound(
        struct Pattern *children,
        enum PatternType type)
{
    struct Pattern *result = mem_malloc(sizeof(*result));

    if (type != PATTERN_ARRAY && type != PATTERN_TUPLE) {
        LOG_ERROR("Compound pattern created with an incorrect type.");
        exit(1);
    }

    result->type = type;
    result->symbol = NULL;
    result->children = children;
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

bool pattern_list_contains_symbol(struct Pattern *pattern, char *symbol)
{
    while (pattern) {
        switch (pattern->type) {
        case PATTERN_SYMBOL:
            if (strcmp(pattern->symbol, symbol) == 0) {
                return true;
            }
            break;

        case PATTERN_ARRAY:
        case PATTERN_TUPLE:
            if (pattern_list_contains_symbol(pattern->children, symbol)) {
                return true;
            }
            break;
        }
        pattern = pattern->next;
    }
    return false;
}

