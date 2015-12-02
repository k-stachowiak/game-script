/* Copyright (C) 2015 Krzysztof Stachowiak */

#include "error.h"
#include "runtime.h"

static void eval_bind_pattern_literal_array(
        struct Runtime *rt,
        struct PatternLiteral *literal,
        VAL_LOC_T location)
{
    VAL_LOC_T first_val = rt_val_cpd_first_loc(location);
    char *val_string;
    if (rt_val_peek_type(&rt->stack, first_val) != VAL_CHAR ||
        literal->type != PATTERN_LIT_STRING) {
        err_push("EVAL", "Failed matching array to literal pattern");
        return;
    }

    val_string = rt_val_peek_cpd_as_string(rt, location);
    if (!strcmp(val_string, literal->data.string)) {
        err_push("EVAL", "Failed matching character array to string pattern");
    }
    mem_free(val_string);
}

static void eval_bind_pattern_literal_atom(
        struct Runtime *rt,
        struct PatternLiteral *literal,
        VAL_LOC_T location,
        enum ValueType value_type)
{
    switch (literal->type) {
    case PATTERN_LIT_UNIT:
        if (value_type == VAL_UNIT) {
            return;
        }
        break;

    case PATTERN_LIT_BOOL:
        if (value_type == VAL_BOOL &&
            rt_val_peek_bool(rt, location) == literal->data.boolean) {
            return;
        }
        break;

    case PATTERN_LIT_CHAR:
        if (value_type == VAL_CHAR &&
            rt_val_peek_char(rt, location) == literal->data.character) {
            return;
        }
        break;

    case PATTERN_LIT_INT:
        if (value_type == VAL_INT &&
            rt_val_peek_int(rt, location) == literal->data.integer) {
            return;
        }
        break;

    case PATTERN_LIT_REAL:
        if (value_type == VAL_REAL &&
            rt_val_peek_real(rt, location) == literal->data.real) {
            return;
        }
        break;

    case PATTERN_LIT_STRING:
        break;
    }

    err_push("EVAL", "Literal to pattern mismatch");
    return;
}

static void eval_bind_pattern_literal(
        struct Runtime *rt,
        struct PatternLiteral *literal,
        VAL_LOC_T location)
{
    enum ValueType value_type = rt_val_peek_type(&rt->stack, location);
    switch (value_type) {
    case VAL_TUPLE:
        err_push("EVAL", "Compound value matched to literal pattern");
        return;
    case VAL_FUNCTION:
        err_push("EVAL", "Function value matched to literal pattern");
        return;
    case VAL_REF:
        err_push("EVAL", "Reference value matched to literal pattern");
        return;

    case VAL_ARRAY:
        eval_bind_pattern_literal_array(rt, literal, location);
        return;

    case VAL_BOOL:
    case VAL_INT:
    case VAL_REAL:
    case VAL_CHAR:
    case VAL_UNIT:
        eval_bind_pattern_literal_atom(rt, literal, location, value_type);
        return;
    }

    LOG_ERROR("Should never get here.");
    exit(1);
}

void eval_bind_pattern(
        struct Pattern *pattern,
        struct Runtime *rt,
        struct SymMap *sym_map,
        VAL_LOC_T location,
        struct SourceLocation *source_loc)
{
    VAL_LOC_T child_loc;
    struct Pattern *child_pat;
    int i, cpd_len, pattern_len;
    enum ValueType type = rt_val_peek_type(&rt->stack, location);

    if (pattern->type == PATTERN_SYMBOL) {
        if (pattern->data.symbol.type == PATTERN_SYM_REGULAR) {
            sym_map_insert(
                sym_map,
                pattern->data.symbol.symbol,
                location,
                *source_loc);
        }
        return;
    }

    if (pattern->type == PATTERN_LITERAL) {
        eval_bind_pattern_literal(rt, &pattern->data.literal, location);
        return;
    }

    if ((pattern->type == PATTERN_CPD_ARRAY && type != VAL_ARRAY) ||
        (pattern->type == PATTERN_CPD_TUPLE && type != VAL_TUPLE)) {
        err_push("EVAL", "Compound type mismatched");
        return;
    }

    cpd_len = rt_val_cpd_len(rt, location);
    pattern_len = pattern_list_len(pattern->data.compound.children);
    if (cpd_len != pattern_len) {
        err_push("EVAL", "Compound bind length mismatched");
        return;
    }

    child_loc = rt_val_cpd_first_loc(location);
    child_pat = pattern->data.compound.children;
    for (i = 0; i < pattern_len; ++i) {
        eval_bind_pattern(child_pat, rt, sym_map, child_loc, source_loc);
        if (err_state()) {
            err_push("EVAL", "Failed evaluating bind pattern");
            return;
        }
        child_loc = rt_val_next_loc(rt, child_loc);
        child_pat = child_pat->next;
    }
}

