/* Copyright (C) 2015 Krzysztof Stachowiak */

#include "error.h"
#include "runtime.h"

static void eval_bind_pattern_literal_atom(
        struct PatternLiteralAtom *literal_atom,
        struct Runtime *rt,
        VAL_LOC_T location,
        struct SourceLocation *source_loc)
{
    enum ValueType value_type = rt_val_peek_type(&rt->stack, location);
    VAL_LOC_T current_loc = rt_val_cpd_first_loc(location);

    switch (literal_atom->type) {
    case PATTERN_LITERAL_ATOM_UNIT:
        if (value_type == VAL_UNIT) {
            return;
        }
        break;

    case PATTERN_LITERAL_ATOM_BOOL:
        if (value_type == VAL_BOOL &&
            rt_val_peek_bool(rt, location) == literal_atom->data.boolean) {
            return;
        }
        break;

    case PATTERN_LITERAL_ATOM_CHAR:
        if (value_type == VAL_CHAR &&
            rt_val_peek_char(rt, location) == literal_atom->data.character) {
            return;
        }
        break;

    case PATTERN_LITERAL_ATOM_INT:
        if (value_type == VAL_INT &&
            rt_val_peek_int(rt, location) == literal_atom->data.integer) {
            return;
        }
        break;

    case PATTERN_LITERAL_ATOM_REAL:
        if (value_type == VAL_REAL &&
            rt_val_peek_real(rt, location) == literal_atom->data.real) {
            return;
        }
        break;

    case PATTERN_LITERAL_ATOM_STRING:
        if (value_type == VAL_ARRAY &&
            rt_val_peek_type(&rt->stack, current_loc) == VAL_CHAR) {
            char *str_pat = literal_atom->data.string;
            int i, len = strlen(str_pat);
            for (i = 0; i < len; ++i) {
                if (*str_pat != rt_val_peek_char(rt, current_loc)) {
                    err_push_src("EVAL", *source_loc, "Failed matching string literal");
                } else {
                    current_loc = rt_val_next_loc(rt, current_loc);
                    ++str_pat;
                }
            }
        }
        break;
    }

    err_push_src("EVAL", *source_loc, "Literal_atom value pattern mismatch");
}

static void eval_bind_pattern_datatype_compound(
        struct PatternDataType *data_type,
        struct Runtime *rt,
        struct SymMap *sym_map,
        VAL_LOC_T location,
        struct SourceLocation *source_loc)
{
    VAL_LOC_T current_loc = rt_val_cpd_first_loc(location);
    struct Pattern *current_pat = data_type->children;
    int val_len = rt_val_cpd_len(rt, location);
    int pat_len = pattern_list_len(current_pat);

    if (current_pat->type == PATTERN_DATATYPE_SEQUENCE) {
        if (pat_len > 1) {
            err_push_src("EVAL", source_loc, "Illegal pattern tokens besides sequence");
            return;
        } else {
            /* It has already been established that the value is of a
             * correct compound type, therefore besides the syntactical check
             * we can call it a day here.
             */
            return;
        }
    }

    if (val_len != pat_len) {
        err_push_src("EVAL", source_loc, "Compound value and pattern length mismatch");
        return;
    }

    while (current_pat) {
        eval_bind_pattern(current_pat, rt, sym_map, current_loc, source_loc);
        if (err_state()) {
            err_push_src("EVAL", *source_loc, "Failed matching one of the array pattern elements");
            return;
        }
        current_pat = current_pat->next;
        current_loc = rt_val_next_loc(rt, current_loc);
    }
}

static void eval_bind_pattern_datatype(
        struct PatternDataType *data_type,
        struct Runtime *rt,
        struct SymMap *sym_map,
        VAL_LOC_T location,
        struct SourceLocation *source_loc)
{
    enum ValueType value_type = rt_val_peek_type(&rt->stack, location);
    switch (data_type->type) {
    case PATTERN_DTYPE_UNIT:
        if (value_type == VAL_UNIT) {
            return;
        }
        break;

    case PATTERN_DTYPE_BOOLEAN:
        if (value_type == VAL_BOOL) {
            return;
        }
        break;

    case PATTERN_DTYPE_INTEGER:
        if (value_type == VAL_INT) {
            return;
        }
        break;

    case PATTERN_DTYPE_REAL:
        if (value_type == VAL_REAL) {
            return;
        }
        break;

    case PATTERN_DTYPE_CHARACTER:
        if (value_type == VAL_CHAR) {
            return;
        }
        break;

    case PATTERN_DTYPE_ARRAY:
    case PATTERN_DTYPE_TUPLE:
        if ((data_type->type == PATTERN_DTYPE_ARRAY && value_type != VAL_ARRAY) ||
            (data_type->type == PATTERN_DTYPE_TUPLE && value_type != VAL_TUPLE)) {
            err_push_src("EVAL", *source_loc, "Compound type pattern mismatch");
        } else {
            eval_bind_pattern_datatype_compound(
                    data_type, rt, sym_map, location, source_loc);
        }
        return;

    case PATTERN_DTYPE_DONT_KNOW:
    case PATTERN_DTYPE_DONT_CARE:
        return;

    case PATTERN_DTYPE_REFERENCE:
        if (value_type != VAL_REF) {
            break;
        }
        eval_bind_pattern_reference(...);
        return;

    case PATTERN_DTYPE_SEQUENCE:
        err_push_src("EVAL", *source_loc, "Unexpected sequence pattern");
        return;
    }
}

void eval_bind_pattern(
        struct Pattern *pattern,
        struct Runtime *rt,
        struct SymMap *sym_map,
        VAL_LOC_T location,
        struct SourceLocation *source_loc)
{
    switch (pattern->type) {
    case PATTERN_DONT_CARE:
        break;

    case PATTERN_SYMBOL:
        sym_map_insert(
            sym_map, pattern->data.symbol.symbol, location, *source_loc);
        break;

    case PATTERN_LITERAL_ATOM:
        eval_bind_pattern_literal_atom(
            &pattern->data.literal_atom, rt, location, source_loc);
        break;

    case PATTERN_LITERAL_COMPOUND:
        eval_bind_pattern_literal_compound(
            &pattern->data.literal_compound, rt, location, source_loc);
        break;

    case PATTERN_DATATYPE:
        eval_bind_pattern_datatype(
            &pattern->data.data_type, rt, sym_map, location, source_loc);
        break;
    }
}

