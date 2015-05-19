/* Copyright (C) 2015 Krzysztof Stachowiak */

#include "collection.h"
#include "mndetail.h"
#include "rt_val.h"

struct Runtime *runtime;

struct MoonValue *mn_make_api_value_compound(VAL_LOC_T loc)
{
    int i, len = rt_val_cpd_len(runtime, loc);
    VAL_LOC_T current_loc = rt_val_cpd_first_loc(loc);
    struct MoonValue *result = NULL, *result_end = NULL;
    for (i = 0; i < len; ++i) {
        struct MoonValue *value = mn_make_api_value(current_loc);
        LIST_APPEND(value, &result, &result_end);
        current_loc = rt_val_next_loc(runtime, current_loc);
    }
    return result;
}

struct MoonValue *mn_make_api_value(VAL_LOC_T loc)
{
    struct MoonValue *result = mem_malloc(sizeof(*result));
    result->next = NULL;

    if (rt_val_is_string(runtime, loc)) {
        char *string = rt_val_peek_cpd_as_string(runtime, loc);
        result->type = MN_STRING;
        result->data.string = string;
        return result;
    }

    switch (rt_val_peek_type(runtime, loc)) {
    case VAL_BOOL:
        result->type = MN_BOOL;
        result->data.boolean = rt_val_peek_bool(runtime, loc);
        break;

    case VAL_CHAR:
        result->type = MN_CHAR;
        result->data.character = rt_val_peek_char(runtime, loc);
        break;

    case VAL_INT:
        result->type = MN_INT;
        result->data.integer = rt_val_peek_int(runtime, loc);
        break;

    case VAL_REAL:
        result->type = MN_REAL;
        result->data.real = rt_val_peek_real(runtime, loc);
        break;

    case VAL_ARRAY:
        result->type = MN_ARRAY;
        result->data.compound = mn_make_api_value_compound(loc);
        break;

    case VAL_TUPLE:
        result->type = MN_TUPLE;
        result->data.compound = mn_make_api_value_compound(loc);
        break;

    case VAL_FUNCTION:
        result->type = MN_FUNCTION;
        break;
    }

    return result;
}

void mn_api_value_free(struct MoonValue *value)
{
    while (value) {

        struct MoonValue *next_value = value->next;

        switch (value->type) {
        case MN_BOOL:
        case MN_CHAR:
        case MN_INT:
        case MN_REAL:
        case MN_FUNCTION:
            break;

        case MN_STRING:
            mem_free(value->data.string);
            break;

        case MN_ARRAY:
        case MN_TUPLE:
            mn_api_value_free(value->data.compound);
            break;
        }

        mem_free(value);
        value = next_value;
    }
}

