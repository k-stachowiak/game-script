/* Copyright (C) 2014-2016 Krzysztof Stachowiak */

#include "error.h"
#include "runtime.h"
#include "eval_detail.h"

static void eval_bind_pattern_datatype(
        struct AstNode *pattern,
        struct Runtime *rt,
        struct SymMap *sym_map,
        VAL_LOC_T location,
        struct AstLocMap *alm);

static void eval_bind_pattern_literal_compound(
        struct AstNode *pattern,
        struct Runtime *rt,
        struct SymMap *sym_map,
        VAL_LOC_T location,
        struct AstLocMap *alm)
{
    VAL_LOC_T current_loc;
    int pat_len, val_len;

    struct AstLiteralCompound *literal_compound = &pattern->data.literal_compound;
    enum AstLiteralCompoundType pat_type = literal_compound->type;
    struct AstNode *current_pat = literal_compound->exprs;

    enum ValueType val_type = rt_val_peek_type(&rt->stack, location);

    if (!(val_type == VAL_ARRAY && pat_type == AST_LIT_CPD_ARRAY) &&
        !(val_type == VAL_TUPLE && pat_type == AST_LIT_CPD_TUPLE)) {
            err_push_src(
                "EVAL",
                alm_get(alm, pattern),
                "Compound value and pattern type mismatch");
            return;
    }

    pat_len = ast_list_len(current_pat);
    val_len = rt_val_cpd_len(rt, location);
    if (val_len != pat_len) {
        err_push_src(
            "EVAL",
            alm_get(alm, pattern),
            "Compound value and pattern length mismatch");
        return;
    }

    current_loc = rt_val_cpd_first_loc(location);
    while (current_pat) {
        eval_special_bind_pattern(current_pat, current_loc, rt, sym_map, alm);
        if (err_state()) {
            err_push_src(
                "EVAL",
                alm_get(alm, current_pat),
                "Failed matching one of the compound pattern elements");
            return;
        }
        current_pat = current_pat->next;
        current_loc = rt_val_next_loc(rt, current_loc);
    }
}

static void eval_bind_pattern_datatype_array(
        struct AstNode *pattern,
        struct Runtime *rt,
        struct SymMap *sym_map,
        VAL_LOC_T location,
        enum ValueType value_type,
        struct AstLocMap *alm)
{
    /* 0. Overview:
     * This algorithm verifies whether a matched value is an array and
     * if it stores values of a type that matches the given criterion.
     */

    int arr_length, pat_length;
    VAL_LOC_T first_loc;
    struct AstNode *child;
    struct AstDatatype *datatype = &pattern->data.datatype;

    if (value_type != VAL_ARRAY) {
        err_push_src(
            "EVAL",
            alm_get(alm, pattern),
            "Failed matching array pattern");
        return;
    }

    arr_length = rt_val_cpd_len(rt, location);
    if (arr_length == 0) {
        /* Always match an empty array */
        return;
    }

    pat_length = ast_list_len(datatype->children);
    if (pat_length != 1) {
        err_push_src(
            "EVAL",
            alm_get(alm, pattern),
            "Failed matching array pattern");
        return;
    }

    child = datatype->children;
    if (child->type != AST_DATATYPE) {
        err_push_src(
            "EVAL",
            alm_get(alm, pattern),
            "Failed matching array pattern");
        return;
    }

    first_loc = rt_val_cpd_first_loc(location);
    eval_bind_pattern_datatype(child, rt, sym_map, first_loc, alm);
}

static void eval_bind_pattern_datatype_reference(
        struct AstNode *pattern,
        struct Runtime *rt,
        struct SymMap *sym_map,
        VAL_LOC_T location,
        enum ValueType value_type,
        struct AstLocMap *alm)
{
    /* 0. Overview:
     * This algorithm verifies whether a matched value is a reference and
     * if it refers to a value that fulfils the inner pattern definition.
     */
    int length;
    VAL_LOC_T refered_loc;
    struct AstNode *child;
    struct AstDatatype *datatype = &pattern->data.datatype;

    if (value_type != VAL_REF) {
        err_push_src(
            "EVAL",
            alm_get(alm, pattern),
            "Failed matching reference pattern");
        return;
    }

    length = ast_list_len(datatype->children);
    if (length != 1) {
        err_push_src(
            "EVAL",
            alm_get(alm, pattern),
            "Failed matching reference pattern");
        return;
    }

    child = datatype->children;
    if (child->type != AST_DATATYPE) {
        err_push_src(
            "EVAL",
            alm_get(alm, child),
            "Failed matching reference pattern");
        return;
    }

    refered_loc = rt_val_peek_ref(rt, location);
    eval_bind_pattern_datatype(child, rt, sym_map, refered_loc, alm);
}

static void eval_bind_pattern_datatype(
        struct AstNode *pattern,
        struct Runtime *rt,
        struct SymMap *sym_map,
        VAL_LOC_T location,
        struct AstLocMap *alm)
{
    /* 0. Overview:
     * This algorithm matches a value against a datatype pattern.
     * In most cases this boils down to the test of the value's type, however
     * in case of the compound type definitions this function must recur.
     * Additionally the verification of the function type is non-trivial.
     */
    struct AstDatatype *datatype = &pattern->data.datatype;
    enum ValueType value_type = rt_val_peek_type(&rt->stack, location);

    switch (datatype->type) {
    case AST_DATATYPE_UNIT:
        if (value_type != VAL_UNIT) {
            err_push_src(
                "EVAL",
                alm_get(alm, pattern),
                "Match failure expected unit");
        }
        break;

    case AST_DATATYPE_BOOLEAN:
        if (value_type != VAL_BOOL) {
            err_push_src(
                "EVAL",
                alm_get(alm, pattern),
                "Match failure expected boolean");
        }
        break;

    case AST_DATATYPE_INTEGER:
        if (value_type != VAL_INT) {
            err_push_src(
                "EVAL",
                alm_get(alm, pattern),
                "Match failure expected integer");
        }
        break;

    case AST_DATATYPE_REAL:
        if (value_type != VAL_REAL) {
            err_push_src(
                "EVAL",
                alm_get(alm, pattern),
                "Match failure expected real");
        }
        break;

    case AST_DATATYPE_CHARACTER:
        if (value_type != VAL_CHAR) {
            err_push_src(
                "EVAL",
                alm_get(alm, pattern),
                "Match failure expected character");
        }
        break;

    case AST_DATATYPE_FUNCTION:
        if (value_type != VAL_FUNCTION) {
            err_push_src(
                "EVAL",
                alm_get(alm, pattern),
                "Match failure expected function");
        } else {
            err_push_src(
                "EVAL",
                alm_get(alm, pattern),
                "Matched function value, but it is not supported yet");
        }
        break;

    case AST_DATATYPE_ARRAY_OF:
        eval_bind_pattern_datatype_array(
            pattern, rt, sym_map, location, value_type, alm);
        break;

    case AST_DATATYPE_REFERENCE_TO:
        eval_bind_pattern_datatype_reference(
            pattern, rt, sym_map, location, value_type, alm);
        break;
    }
}

void eval_special_bind_pattern_evaluable(
        struct AstNode *pattern,
        struct Runtime *rt,
        struct SymMap *sym_map,
        VAL_LOC_T location,
        struct AstLocMap *alm)
{
    /* 0. Overview:
     * This algoritm evaluates the pattern expression temporarily and tests
     * if the matched value is equal to the pattern expression evaluation
     * results. This actiom makes sense for a selection of possible pattern
     * expressions.
     */

    VAL_LOC_T test_loc, temp_begin, temp_end;

    temp_begin = rt->stack.top;
    test_loc = eval_dispatch(pattern, rt, sym_map, alm);
    temp_end = rt->stack.top;

    if (!rt_val_eq_rec(rt, test_loc, location)) {
        err_push_src(
            "EVAL",
            alm_get(alm, pattern),
            "Failed matching pattern against a value");
    }

    stack_collapse(&rt->stack, temp_begin, temp_end);
}

void eval_special_bind_pattern(
        struct AstNode *pattern,
        VAL_LOC_T location,
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct AstLocMap *alm)
{
    /* 0. Summary:
     * This algorithm performs the matching of an already evaluated value
     * against a pattern. Since the pattern is allowed to be any expression
     * (from the syntactical point of view), the algorithm is not trivial.
     * In the explanation below "no side effects" means that no side effects
     * are generated by this algorithms, however this doesn't include the
     * side effects of the efaluation of the pattern expression. In this
     * context the term of the side effects is used to distinguish between
     * the active pattern matching - which e.g. introduces a new variable and
     * the passive pattern matching - which only tests the variable's value
     * or structure.
     */

    /* 1. Recognize the type of the pattern expression and perform an according
     * action.
     */
    switch (pattern->type) {
    case AST_SYMBOL:
        /* 1.1. Matching against a symbol is a variable definition. */
        sym_map_insert(sym_map, pattern->data.symbol.symbol, location);
        break;

    case AST_SPECIAL:
    case AST_FUNCTION_CALL:
    case AST_LITERAL_ATOMIC:
        /* 1.2. Matching against evaluable expressions means comparison, i.e.
         * the matching expression will not have side effects in terms of
         * creation of a new variable or an assignment, but at the same time
         * will fail if the result of the pattern evaluation is not equal to
         * the matched value.
         */
        eval_special_bind_pattern_evaluable(pattern, rt, sym_map, location, alm);
        break;

    case AST_LITERAL_COMPOUND:
        /* 1.3. Matching against a compound expression means the test of the
         * structure of the values followed by the recursive matching operation
         * for each of the pattern members.
         */
        eval_bind_pattern_literal_compound(pattern, rt, sym_map, location, alm);
        break;

    case AST_DATATYPE:
        /* 1.4. Matching against a datatype only verifies the type of the
         * matched value, regardless of the value. No side effects happen upon
         * this operation.
         */
        eval_bind_pattern_datatype(pattern, rt, sym_map, location, alm);
        break;
    }
}

