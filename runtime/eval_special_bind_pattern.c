/* Copyright (C) 2014-2016 Krzysztof Stachowiak */

#include "error.h"
#include "runtime.h"
#include "eval_detail.h"

static void eval_bind_pattern_datatype(
        struct Pattern *pattern,
        struct Runtime *rt,
        struct SymMap *sym_map,
        VAL_LOC_T location,
    struct AstLocMap *alm);

static void eval_bind_pattern_literal_atomic(
        struct Pattern *pattern,
        struct Runtime *rt,
        VAL_LOC_T location,
    struct AstLocMap *alm)
{
    enum ValueType value_type = rt_val_peek_type(&rt->stack, location);
    VAL_LOC_T current_loc = rt_val_cpd_first_loc(location);
    struct PatternLiteralAtomic *literal_atomic = &pattern->data.literal_atomic;

    switch (literal_atomic->type) {
    case PATTERN_LIT_ATOM_UNIT:
        if (value_type == VAL_UNIT) {
            return;
        }
        break;

    case PATTERN_LIT_ATOM_BOOL:
        if (value_type == VAL_BOOL &&
            rt_val_peek_bool(rt, location) == literal_atomic->data.boolean) {
            return;
        }
        break;

    case PATTERN_LIT_ATOM_CHAR:
        if (value_type == VAL_CHAR &&
            rt_val_peek_char(rt, location) == literal_atomic->data.character) {
            return;
        }
        break;

    case PATTERN_LIT_ATOM_INT:
        if (value_type == VAL_INT &&
            rt_val_peek_int(rt, location) == literal_atomic->data.integer) {
            return;
        }
        break;

    case PATTERN_LIT_ATOM_REAL:
        if (value_type == VAL_REAL &&
            rt_val_peek_real(rt, location) == literal_atomic->data.real) {
            return;
        }
        break;

    case PATTERN_LIT_ATOM_STRING:
        if (value_type == VAL_ARRAY && rt_val_peek_type(&rt->stack, current_loc) == VAL_CHAR) {
            char *str_pat = literal_atomic->data.string;
            int arr_len = rt_val_cpd_len(rt, location);
            int str_len = strlen(str_pat) - 2; /* Take the quotes into account */

            if (str_len != arr_len) {
                err_push_src(
            "EVAL",
            alm_get_pat(alm, pattern),
            "Failed matching string literal");
                return;
            }

            ++str_pat; /* Skip the initial quote */
            while (arr_len) {
                if (*str_pat != rt_val_peek_char(rt, current_loc)) {
                    err_push_src(
            "EVAL",
            alm_get_pat(alm, pattern),
            "Failed matching string literal");
                    return;
                }
                current_loc = rt_val_next_loc(rt, current_loc);
                ++str_pat;
                --arr_len;
            }
            return;
        }
        break;
    }

    err_push_src(
    "EVAL",
    alm_get_pat(alm, pattern),
    "Literal_atomic value pattern mismatch");
}

static void eval_bind_pattern_literal_compound(
        struct Pattern *pattern,
        struct Runtime *rt,
        struct SymMap *sym_map,
        VAL_LOC_T location,
    struct AstLocMap *alm)
{
    struct PatternLiteralCompound *literal_compound = &pattern->data.literal_compound;
    VAL_LOC_T current_loc = rt_val_cpd_first_loc(location);

    enum ValueType val_type = rt_val_peek_type(&rt->stack, location);
    int val_len = rt_val_cpd_len(rt, location);

    enum PatternLiteralCompoundType pat_type = literal_compound->type;
    struct Pattern *current_pat = literal_compound->children;
    int pat_len = pattern_list_len(current_pat);

    if (!(val_type == VAL_ARRAY && pat_type == PATTERN_LIT_CPD_ARRAY) &&
        !(val_type == VAL_TUPLE && pat_type == PATTERN_LIT_CPD_TUPLE)) {
            err_push_src(
        "EVAL",
        alm_get_pat(alm, pattern),
        "Compound value and pattern type mismatch");
            return;
    }

    if (val_len != pat_len) {
        err_push_src(
        "EVAL",
        alm_get_pat(alm, pattern),
        "Compound value and pattern length mismatch");
        return;
    }

    while (current_pat) {
        eval_special_bind_pattern(current_pat, current_loc, rt, sym_map, alm);
        if (err_state()) {
            err_push_src(
        "EVAL",
        alm_get_pat(alm, current_pat),
        "Failed matching one of the compound pattern elements");
            return;
        }
        current_pat = current_pat->next;
        current_loc = rt_val_next_loc(rt, current_loc);
    }
}

static void eval_bind_pattern_datatype_array(
        struct Pattern *pattern,
        struct Runtime *rt,
        struct SymMap *sym_map,
        VAL_LOC_T location,
        enum ValueType value_type,
    struct AstLocMap *alm)
{
    int arr_length, pat_length;
    VAL_LOC_T first_loc;
    struct Pattern *child;
    struct PatternDataType *datatype = &pattern->data.datatype;

    if (value_type != VAL_ARRAY) {
        err_push_src(
        "EVAL",
        alm_get_pat(alm, pattern),
        "Failed matching array pattern");
        return;
    }

    arr_length = rt_val_cpd_len(rt, location);
    if (arr_length == 0) {
        /* Always match an empty array */
        return;
    }

    pat_length = pattern_list_len(datatype->children);
    if (pat_length != 1) {
        err_push_src(
        "EVAL",
        alm_get_pat(alm, pattern),
        "Failed matching array pattern");
        return;
    }

    child = datatype->children;
    if (child->type != PATTERN_DATATYPE) {
        err_push_src(
        "EVAL",
        alm_get_pat(alm, pattern),
        "Failed matching array pattern");
        return;
    }

    first_loc = rt_val_cpd_first_loc(location);
    eval_bind_pattern_datatype(child, rt, sym_map, first_loc, alm);
}

static void eval_bind_pattern_datatype_reference(
        struct Pattern *pattern,
        struct Runtime *rt,
        struct SymMap *sym_map,
        VAL_LOC_T location,
        enum ValueType value_type,
    struct AstLocMap *alm)
{
    int length;
    VAL_LOC_T refered_loc;
    struct Pattern *child;
    struct PatternDataType *datatype = &pattern->data.datatype;

    if (value_type != VAL_REF) {
        err_push_src(
        "EVAL",
        alm_get_pat(alm, pattern),
        "Failed matching reference pattern");
        return;
    }

    length = pattern_list_len(datatype->children);
    if (length != 1) {
        err_push_src(
        "EVAL",
        alm_get_pat(alm, pattern),
        "Failed matching reference pattern");
        return;
    }

    child = datatype->children;
    if (child->type != PATTERN_DATATYPE) {
        err_push_src(
        "EVAL",
        alm_get_pat(alm, child),
        "Failed matching reference pattern");
        return;
    }

    refered_loc = rt_val_peek_ref(rt, location);
    eval_bind_pattern_datatype(child, rt, sym_map, refered_loc, alm);
}

static void eval_bind_pattern_datatype(
        struct Pattern *pattern,
        struct Runtime *rt,
        struct SymMap *sym_map,
        VAL_LOC_T location,
    struct AstLocMap *alm)
{
    struct PatternDataType *datatype = &pattern->data.datatype;
    enum ValueType value_type = rt_val_peek_type(&rt->stack, location);

    switch (datatype->type) {
    case PATTERN_DATATYPE_UNIT:
        if (value_type != VAL_UNIT) {
            err_push_src(
        "EVAL",
        alm_get_pat(alm, pattern),
        "Match failure expected unit");
        }
        break;
    case PATTERN_DATATYPE_BOOLEAN:
        if (value_type != VAL_BOOL) {
            err_push_src(
        "EVAL", 
        alm_get_pat(alm, pattern),
        "Match failure expected boolean");
        }
        break;
    case PATTERN_DATATYPE_INTEGER:
        if (value_type != VAL_INT) {
            err_push_src(
        "EVAL",
        alm_get_pat(alm, pattern),
        "Match failure expected integer");
        }
        break;
    case PATTERN_DATATYPE_REAL:
        if (value_type != VAL_REAL) {
            err_push_src(
        "EVAL",
        alm_get_pat(alm, pattern),
        "Match failure expected real");
        }
        break;
    case PATTERN_DATATYPE_CHARACTER:
        if (value_type != VAL_CHAR) {
            err_push_src(
        "EVAL",
        alm_get_pat(alm, pattern),
        "Match failure expected character");
        }
        break;
    case PATTERN_DATATYPE_FUNCTION:
        if (value_type != VAL_FUNCTION) {
            err_push_src(
        "EVAL",
        alm_get_pat(alm, pattern),
        "Match failure expected function");
        } else {
            err_push_src(
        "EVAL",
        alm_get_pat(alm, pattern),
        "Matched function value, but it is not supported yet");
        }
        break;
    case PATTERN_DATATYPE_ARRAY_OF:
        eval_bind_pattern_datatype_array(
        pattern, rt, sym_map, location, value_type, alm);
        break;
    case PATTERN_DATATYPE_REFERENCE_TO:
        eval_bind_pattern_datatype_reference(
        pattern, rt, sym_map, location, value_type, alm);
        break;
    }
}

void eval_special_bind_pattern(
        struct Pattern *pattern,
        VAL_LOC_T location,
        struct Runtime *rt,
        struct SymMap *sym_map,
    struct AstLocMap *alm)
{
    switch (pattern->type) {
    case PATTERN_DONT_CARE:
        break;

    case PATTERN_SYMBOL:
        sym_map_insert(sym_map, pattern->data.symbol.symbol, location);
        break;

    case PATTERN_LITERAL_ATOMIC:
        eval_bind_pattern_literal_atomic(pattern, rt, location, alm);
        break;

    case PATTERN_LITERAL_COMPOUND:
        eval_bind_pattern_literal_compound(pattern, rt, sym_map, location, alm);
        break;

    case PATTERN_DATATYPE:
        eval_bind_pattern_datatype(pattern, rt, sym_map, location, alm);
        break;
    }
}

