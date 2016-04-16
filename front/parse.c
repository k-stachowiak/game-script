/* Copyright (C) 2014,2015 Krzysztof Stachowiak */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "term.h"
#include "log.h"
#include "collection.h"
#include "error.h"
#include "parse.h"
#include "memory.h"
#include "tok.h"
#include "lex.h"
#include "dom.h"
#include "pattern.h"

/* Algorithms.
 * ===========
 */

static char *find(char *current, char *last, char value)
{
    while (current != last) {
        if (*current == value) {
            return current;
        }
        ++current;
    }

    return current;
}

static bool all_of(char *current, char *last, int (*f)(int))
{
    while (current != last) {
        if (!f(*current)) {
            return false;
        }
        ++current;
    }
    return true;
}

static char *unescape_string(char *in)
{
    int in_len = strlen(in);
    char *out = mem_calloc(in_len + 1, 1);
    char *write = out;
    while (*in) {

        if (*in != '\\') {
            *write++ = *in++;
            continue;
        }

        switch (*++in) {
            case 'a':
                *write++ = '\a';
                break;
            case 'b':
                *write++ = '\b';
                break;
            case 't':
                *write++ = '\t';
                break;
            case 'n':
                *write++ = '\n';
                break;
            case 'f':
                *write++ = '\f';
                break;
            case 'r':
                *write++ = '\r';
                break;
            case 'v':
                *write++ = '\v';
                break;
            case '\\':
                *write++ = '\\';
                break;
            case '"':
                *write++ = '"';
                break;
            case '\0':
                *write++ = '\0';
                break;
            default:
                mem_free(out);
                return NULL;
        }
        ++in;
    }

    return mem_realloc(out, write - out + 1);
}

static bool parse_string(char *string, char **result)
{
    int len = strlen(string);

    if (len < 2) {
        return false;
    }

    if (string[0] == TOK_DELIM_STR && string[len - 1] == TOK_DELIM_STR) {
        char *unescaped = unescape_string(string);
        if (!unescaped) {
            return false;
        } else {
            *result = unescaped;
            return true;
        }

    } else {
        return false;
    }
}

static bool parse_char(char *string, char *result)
{
    int len = strlen(string);

    if (string[0] != TOK_DELIM_CHAR || string[len - 1] != TOK_DELIM_CHAR) {
        return false;
    }

    if (len == 3) {
        *result = string[1];
        return true;

    } else if (len == 4 && string[1] == TOK_DELIM_ESCAPE) {
        switch(string[2]) {
         case 'a':
            *result = '\a';
            return true;
         case 'b':
            *result = '\b';
            return true;
         case 't':
            *result = '\t';
            return true;
         case 'n':
            *result = '\n';
            return true;
         case 'f':
            *result = '\f';
            return true;
         case 'r':
            *result = '\r';
            return true;
         case 'v':
            *result = '\v';
            return true;
         case '\\':
            *result = '\\';
            return true;
         case '\'':
            *result = '\'';
            return true;
         case '0':
            *result = '\0';
            return true;
         default:
            return false;
        }

    } else {
        return false;

    }
}

static bool parse_int(char *string, long *result)
{
    char *first, *last;
    int len = strlen(string);

    if (len == 0) {
        return false;
    }

    first = string;
    last = string + len;

    if (string[0] == '-' || string[0] == '+') {
        ++first;
    }

    if (first == last) {
        return false;
    }

    if (all_of(first, last, isdigit)) {
        *result = atol(string);
        return true;

    } else {
        return false;
    }
}

static bool parse_real(char *string, double *result)
{
    char *first, *period, *last;
    int len = strlen(string);

    first = string;
    last = first + len;
    period = find(first, last, '.');

    if (string[0] == '-' || string[0] == '+') {
        ++first;
    }

    if (first == last) {
        return false;
    }

    if (period == last ||
        !all_of(first, period, isdigit) ||
        !all_of(period + 1, last, isdigit)) {
        return false;
    }

    *result = atof(string);
    return true;
}

/* Pattern parsing.
 * ================
 */

static struct Pattern *parse_pattern(struct DomNode *dom);

static struct Pattern *parse_pattern_list(struct DomNode *dom)
{
    struct Pattern *result = NULL, *result_end = NULL;
    while (dom) {
        struct Pattern *item = parse_pattern(dom);
        if (!item) {
            pattern_free(result);
            return NULL;
        }
        LIST_APPEND(item, &result, &result_end);
        dom = dom->next;
    }
    return result;
}

static struct Pattern *parse_pattern_dont_care(struct DomNode *dom)
{
    if (dom_node_is_spec_reserved_atom(dom, DOM_RES_DONT_CARE)) {
        return pattern_make_dont_care();
    } else {
        return NULL;
    }
}

static struct Pattern *parse_pattern_symbol(struct DomNode *dom)
{
    char *symbol;
    if ((symbol = dom_node_parse_symbol(dom))) {
        struct Pattern *result = pattern_make_symbol(symbol);
        mem_free(symbol);
        return result;
    } else {
        return NULL;
    }
}

static struct Pattern *parse_pattern_literal_atomic_unit(struct DomNode *dom)
{
    if (dom_node_is_spec_reserved_atom(dom, DOM_RES_UNIT)) {
        return pattern_make_literal_atomic_unit();
    } else {
        return NULL;
    }
}

static struct Pattern *parse_pattern_literal_atomic_bool(struct DomNode *dom)
{
    if (dom_node_is_spec_reserved_atom(dom, DOM_RES_TRUE)) {
        return pattern_make_literal_atomic_bool(1);
    } else if (dom_node_is_spec_reserved_atom(dom, DOM_RES_FALSE)) {
        return pattern_make_literal_atomic_bool(0);
    } else {
        return NULL;
    }
}

static struct Pattern *parse_pattern_literal_atomic_int(struct DomNode *dom)
{
    long value;

    if (!dom_node_is_atom(dom)) {
        return NULL;
    }

    if (!parse_int(dom->atom, &value)) {
        return NULL;
    }

    return pattern_make_literal_atomic_int(value);
}

static struct Pattern *parse_pattern_literal_atomic_real(struct DomNode *dom)
{
    double value;

    if (!dom_node_is_atom(dom)) {
        return NULL;
    }

    if (!parse_real(dom->atom, &value)) {
        return NULL;
    }

    return pattern_make_literal_atomic_real(value);
}

static struct Pattern *parse_pattern_literal_atomic_char(struct DomNode *dom)
{
    char value;

    if (!dom_node_is_atom(dom)) {
        return NULL;
    }

    if (!parse_char(dom->atom, &value)) {
        return NULL;
    }

    return pattern_make_literal_atomic_character(value);
}

static struct Pattern *parse_pattern_literal_atomic_string(struct DomNode *dom)
{
    char *value;
    struct Pattern *result;

    if (!dom_node_is_atom(dom)) {
        return NULL;
    }

    if (!parse_string(dom->atom, &value)) {
        return NULL;
    }

    result = pattern_make_literal_atomic_string(value);
    mem_free(value);
    return result;
}

static struct Pattern *parse_pattern_literal_atomic(struct DomNode *dom)
{
    struct Pattern *result;

    if ((!err_state() && (result = parse_pattern_literal_atomic_unit(dom))) ||
        (!err_state() && (result = parse_pattern_literal_atomic_bool(dom))) ||
        (!err_state() && (result = parse_pattern_literal_atomic_int(dom))) ||
        (!err_state() && (result = parse_pattern_literal_atomic_real(dom))) ||
        (!err_state() && (result = parse_pattern_literal_atomic_char(dom))) ||
        (!err_state() && (result = parse_pattern_literal_atomic_string(dom)))) {
        return result;
    } else {
        return NULL;
    }
}

static struct Pattern *parse_pattern_literal_compound(struct DomNode *dom)
{
    if (dom_node_is_spec_compound(dom, DOM_CPD_ARRAY)) {
        struct Pattern *children = parse_pattern_list(dom->cpd_children);
        if (!children) {
            return NULL;
        } else {
            return pattern_make_literal_compound_array(children);
        }
    } else if (dom_node_is_spec_compound(dom, DOM_CPD_TUPLE)) {
        struct Pattern *children = parse_pattern_list(dom->cpd_children);
        if (!children) {
            return NULL;
        } else {
            return pattern_make_literal_compound_tuple(children);
        }
    } else {
        return NULL;
    }
}

static struct Pattern *parse_pattern_datatype_atomic(struct DomNode *dom)
{
    if (dom_node_is_spec_reserved_atom(dom, DOM_RES_TUNIT)) {
        return pattern_make_datatype_unit();

    } else if (dom_node_is_spec_reserved_atom(dom, DOM_RES_BOOLEAN)) {
        return pattern_make_datatype_bool();

    } else if (dom_node_is_spec_reserved_atom(dom, DOM_RES_INTEGER)) {
        return pattern_make_datatype_int();

    } else if (dom_node_is_spec_reserved_atom(dom, DOM_RES_REAL)) {
        return pattern_make_datatype_real();

    } else if (dom_node_is_spec_reserved_atom(dom, DOM_RES_CHARACTER)) {
        return pattern_make_datatype_character();

    } else {
        return NULL;

    }
}

static struct Pattern *parse_pattern_datatype_compound(struct DomNode *dom)
{
    struct DomNode *child = NULL;
    struct Pattern *pattern_children = NULL;

    /* 1. Is compound CORE */
    if (!dom_node_is_spec_compound(dom, DOM_CPD_CORE)) {
        return NULL;
    }

    /* 2. Has at least two children */
    if (!dom_node_is_cpd_min_size(dom, 2)) {
        return NULL;
    }

    /* 3. Is of one of three legal types */
    child = dom->cpd_children;
    if (dom_node_is_spec_reserved_atom(child, DOM_RES_ARRAY_OF) &&
            dom_list_length(child) == 2) {
        /* 3.a) Array of - only one pattern argument allowed */
        pattern_children = parse_pattern(child->next);
        if (pattern_children) {
            return pattern_make_datatype_array_of(pattern_children);
        } else {
            return NULL;
        }
    } else if (dom_node_is_spec_reserved_atom(child, DOM_RES_TREFERENCE) &&
            dom_list_length(child) == 2) {
        /* 3.b) Reference to - only one pattern argument allowed */
        pattern_children = parse_pattern(child->next);
        if (pattern_children) {
            return pattern_make_datatype_reference_to(pattern_children);
        } else {
            return NULL;
        }
    } else if (dom_node_is_spec_reserved_atom(child, DOM_RES_TFUNC) &&
            dom_list_length(child) >= 2) {
        /* 3.c) Function - at least two additional pattern arguments required */
        pattern_children = parse_pattern_list(child->next);
        if (pattern_children) {
            return pattern_make_datatype_function(pattern_children);
        } else {
            return NULL;
        }
    } else {
        return NULL;
    }
}

static struct Pattern *parse_pattern_datatype(struct DomNode *dom)
{
    struct Pattern *result = NULL;

    if ((!err_state() && (result = parse_pattern_datatype_atomic(dom))) ||
        (!err_state() && (result = parse_pattern_datatype_compound(dom)))) {
        return result;
    } else {
        return NULL;
    }
}

static struct Pattern *parse_pattern(struct DomNode *dom)
{
    struct Pattern *result = NULL;

    if ((!err_state() && (result = parse_pattern_dont_care(dom))) ||
        (!err_state() && (result = parse_pattern_symbol(dom))) ||
        (!err_state() && (result = parse_pattern_literal_atomic(dom))) ||
        (!err_state() && (result = parse_pattern_literal_compound(dom))) ||
        (!err_state() && (result = parse_pattern_datatype(dom)))) {
        return result;
    } else {
        return NULL;
    }
}

/* Regular AST parsing.
 * ====================
 */

static struct AstNode *parse_one(struct DomNode *dom);
static struct AstNode *parse_list(struct DomNode *dom);

static struct AstNode *parse_do_block(struct DomNode *dom)
{
    struct AstNode *exprs = NULL;
    struct DomNode *child = NULL;

    /* 1. Is compound CORE. */
    if (!dom_node_is_spec_compound(dom, DOM_CPD_CORE)) {
        return NULL;
    }

    /* 2. Has 2 or more children. */
    if (!dom_node_is_cpd_min_size(dom, 2)) {
        return NULL;
    }

    child = dom->cpd_children;

    /* 3.1. 1st child is "do" keyword. */
    if (!dom_node_is_spec_reserved_atom(child, DOM_RES_DO)) {
        return NULL;
    }
    child = child->next;

    /* 3.2. Has 1 or more further expressions. */
    if (!(exprs = parse_list(child))) {
        return NULL;
    }

    return ast_make_ctl_do(&dom->loc, exprs);
}

static struct AstNode *parse_bind(struct DomNode *dom)
{
    struct DomNode *child = NULL;
    struct Pattern *pattern = NULL;
    struct AstNode *expr = NULL;

    /* 1. Is compound CORE */
    if (!dom_node_is_spec_compound(dom, DOM_CPD_CORE)) {
        return NULL;
    }

    /* 2. Has 3 children */
    if (!dom_node_is_cpd_of_size(dom, 3)) {
        return NULL;
    }

    child = dom->cpd_children;

    /* 3.1. 1st child is bind keyword. */
    if (!dom_node_is_spec_reserved_atom(child, DOM_RES_BIND)) {
        return NULL;
    }
    child = child->next;

    /* 3.2. 2nd child is pattern. */
    if (!(pattern = parse_pattern(child))) {
        return NULL;
    }
    child = child->next;

    /* 3.3 3rd child is any expression. */
    if (!(expr = parse_one(child))) {
        pattern_free(pattern);
        return NULL;
    }

    return ast_make_ctl_bind(&dom->loc, pattern, expr);
}

static struct AstNode *parse_special(struct DomNode *dom)
{
    struct DomNode *child = NULL;
    struct AstNode *args = NULL;

    /* 1. Is compound CORE */
    if (!dom_node_is_spec_compound(dom, DOM_CPD_CORE)) {
        return NULL;
    }

    /* 2. Has 1 or more children. */
    if (!dom_node_is_cpd_min_size(dom, 1)) {
        return NULL;
    }

    child = dom->cpd_children;

    /* 3. Switch the first child. */
    if (!dom_node_is_atom(child)) {
        return NULL;
    } else {
        args = parse_list(child->next);
    }

    /* 3.1. Case &&: */
    if (dom_node_is_spec_reserved_atom(child, DOM_RES_AND)) {
        return ast_make_special(&dom->loc, AST_SPECIAL_AND, args);
    }

    /* 3.2. Case ||: */
    if (dom_node_is_spec_reserved_atom(child, DOM_RES_OR)) {
        return ast_make_special(&dom->loc, AST_SPECIAL_OR, args);
    }

    /* 3.3. Case if: */
    if (dom_node_is_spec_reserved_atom(child, DOM_RES_IF)) {
        return ast_make_special(&dom->loc, AST_SPECIAL_IF, args);
    }

    /* 3.4. Case while: */
    if (dom_node_is_spec_reserved_atom(child, DOM_RES_WHILE)) {
        return ast_make_special(&dom->loc, AST_SPECIAL_WHILE, args);
    }

    /* 3.6. Case ref: */
    if (dom_node_is_spec_reserved_atom(child, DOM_RES_REF)) {
        return ast_make_special(&dom->loc, AST_SPECIAL_REF, args);
    }

    /* 3.7. Case peek: */
    if (dom_node_is_spec_reserved_atom(child, DOM_RES_PEEK)) {
        return ast_make_special(&dom->loc, AST_SPECIAL_PEEK, args);
    }

    /* 3.8. Case poke: */
    if (dom_node_is_spec_reserved_atom(child, DOM_RES_POKE)) {
        return ast_make_special(&dom->loc, AST_SPECIAL_POKE, args);
    }

    /* 3.9. Case begin: */
    if (dom_node_is_spec_reserved_atom(child, DOM_RES_BEGIN)) {
        return ast_make_special(&dom->loc, AST_SPECIAL_BEGIN, args);
    }

    /* 3.10. Case end: */
    if (dom_node_is_spec_reserved_atom(child, DOM_RES_END)) {
        return ast_make_special(&dom->loc, AST_SPECIAL_END, args);
    }

    /* 3.11. Case inc: */
    if (dom_node_is_spec_reserved_atom(child, DOM_RES_INC)) {
        return ast_make_special(&dom->loc, AST_SPECIAL_INC, args);
    }

    /* 3.12. Case succ: */
    if (dom_node_is_spec_reserved_atom(child, DOM_RES_SUCC)) {
        return ast_make_special(&dom->loc, AST_SPECIAL_SUCC, args);
    }

    /* None of the reserved words were matched. */
    ast_node_free(args);
    return NULL;
}

static struct AstNode *parse_literal_compound(struct DomNode *dom)
{
    enum AstLiteralCompoundType type;
    struct AstNode *exprs = NULL;

    /* 1. Is compound. */
    if (dom_node_is_atom(dom)) {
        return NULL;
    }

    /* 2. Is of ARRAY or TUPLE type. */
    switch (dom->cpd_type) {
    case DOM_CPD_ARRAY:
        type = AST_LIT_CPD_ARRAY;
        break;

    case DOM_CPD_TUPLE:
        type = AST_LIT_CPD_TUPLE;
        break;

    case DOM_CPD_CORE:
    default:
        return NULL;
    }

    /* 3. Has 0 or more expressions. */
    exprs = parse_list(dom->cpd_children);
    if (err_state()) {
        return NULL;
    } else {
        return ast_make_literal_compound(&dom->loc, type, exprs);
    }
}

static struct AstNode *parse_fcall(struct DomNode *dom)
{
    struct AstNode *func = NULL;
    struct AstNode *args = NULL;
    struct DomNode *child = NULL;

    /* 1. Is compound CORE. */
    if (!dom_node_is_spec_compound(dom, DOM_CPD_CORE)) {
        return NULL;
    }

    /* 2. Has 1 or more children. */
    if (!dom_node_is_cpd_min_size(dom, 1)) {
        return NULL;
    }

    child = dom->cpd_children;

    /* 3.1. 1st child is an non-keyword expression. */
    if (dom_node_is_reserved_atom(child)) {
        return NULL;
    }
    func = parse_one(child);
    if (err_state()) {
        return NULL;
    }
    child = child->next;

    /* 3.2. Has 0 or more further children being any expression. */
    args = parse_list(child);
    if (err_state()) {
        return NULL;
    }

    return ast_make_ctl_fcall(&dom->loc, func, args);
}

static struct AstNode *parse_fdef(struct DomNode *dom)
{
    struct {
        struct SourceLocation *data;
        int cap, size;
    } arg_locs = { NULL, 0, 0 };

    struct AstNode *expr = NULL;

    struct DomNode *child = NULL;
    struct DomNode *arg_child = NULL;

    struct Pattern *formal_args = NULL, *formal_args_end = NULL;

    /* 1. Is compound CORE. */
    if (!dom_node_is_spec_compound(dom, DOM_CPD_CORE)) {
        return NULL;
    }

    /* 2. Has 3 children. */
    if (!dom_node_is_cpd_of_size(dom, 3)) {
        return NULL;
    }

    child = dom->cpd_children;

    /* 3.1. 1st child is "func" keyword. */
    if (!dom_node_is_spec_reserved_atom(child, DOM_RES_FUNC)) {
        return NULL;
    }
    child = child->next;

    /* 3.2. 2nd keyword is a core compound of patterns. */
    if (!dom_node_is_spec_compound(child, DOM_CPD_CORE)) {
        return NULL;
    }

    arg_child = child->cpd_children;

    /* Argument list may be empty. */
    if (arg_child) {
        while (arg_child) {
            struct Pattern *pattern;
            if (!(pattern = parse_pattern(arg_child))) {
                goto fail;

            } else {
                LIST_APPEND(pattern, &formal_args, &formal_args_end);
                ARRAY_APPEND(arg_locs, arg_child->loc);
            }
            arg_child = arg_child->next;
        }
    }

    child = child->next;

    /* 3.3. Has 1 more further expression. */
    if (!(expr = parse_one(child))) {
        goto fail;
    }

    return ast_make_ctl_fdef(
        &dom->loc,
        formal_args,
        mem_realloc(
            arg_locs.data,
            arg_locs.size * sizeof(*arg_locs.data)),
        arg_locs.size,
        expr);

fail:
    if (formal_args) {
        pattern_free(formal_args);
    }

    if (arg_locs.size) {
        ARRAY_FREE(arg_locs);
    }

    return NULL;
}

static struct AstNode *parse_match(struct DomNode *dom)
{
    struct DomNode *child = NULL;
    struct AstNode *expr = NULL;
    struct Pattern *keys = NULL, *keys_end = NULL;
    struct AstNode *values = NULL, *values_end = NULL;

    /* 1. Is compound CORE. */
    if (!dom_node_is_spec_compound(dom, DOM_CPD_CORE)) {
        return NULL;
    }

    /* 2. Has 3 or more children. */
    if (!dom_node_is_cpd_min_size(dom, 3)) {
        return NULL;
    }

    child = dom->cpd_children;

    /* 3.1. 1st child is "match" keyword. */
    if (!dom_node_is_spec_reserved_atom(child, DOM_RES_MATCH)) {
        return NULL;
    }
    child = child->next;

    /* 3.2. 2nd child is an expression. */
    if (!(expr = parse_one(child))) {
        return NULL;
    }
    child = child->next;

    /* 3.3. Has at least one matching expression. */
    while (child) {
        struct DomNode *match_child = NULL;
        struct Pattern *key = NULL;
        struct AstNode *value = NULL;

        /* 3.3.1. Is compound CORE. */
        if (!dom_node_is_spec_compound(child, DOM_CPD_CORE)) {
            goto fail;
        }

        /* 3.3.2. Has 2 children. */
        if (!dom_node_is_cpd_of_size(child, 2)) {
            goto fail;
        }
        match_child = child->cpd_children;

        /* 3.3.3. 1st child is a pattern. */
        if (!(key = parse_pattern(match_child))) {
            goto fail;
        }
        match_child = match_child->next;

        /* 3.3.4. 2nd child is an expression. */
        if (!(value = parse_one(match_child))) {
            goto fail;
        }

        LIST_APPEND(key, &keys, &keys_end);
        LIST_APPEND(value, &values, &values_end);

        child = child->next;
    }

    return ast_make_ctl_match(&dom->loc, expr, keys, values);

fail:
    ast_node_free(expr);

    if (keys) {
        pattern_free(keys);
    }

    if (values) {
        ast_node_free(values);
    }

    return NULL;
}

static struct AstNode *parse_literal_unit(struct DomNode *dom)
{
    if (dom_node_is_spec_reserved_atom(dom, DOM_RES_UNIT)) {
        return ast_make_literal_atomic_unit(&dom->loc);
    } else {
        return NULL;
    }
}

static struct AstNode *parse_literal_bool(struct DomNode *dom)
{
    if (dom_node_is_spec_reserved_atom(dom, DOM_RES_TRUE)) {
        return ast_make_literal_atomic_bool(&dom->loc, 1);
    } else if (dom_node_is_spec_reserved_atom(dom, DOM_RES_FALSE)) {
        return ast_make_literal_atomic_bool(&dom->loc, 0);
    } else {
        return NULL;
    }
}

static struct AstNode *parse_literal_string(struct DomNode *dom)
{
    char *value;
    struct AstNode *result;

    if (!dom_node_is_atom(dom)) {
        return NULL;
    }

    if (!parse_string(dom->atom, &value)) {
        return NULL;
    }

    result = ast_make_literal_atomic_string(&dom->loc, value);
    mem_free(value);
    return result;
}

static struct AstNode *parse_literal_char(struct DomNode *dom)
{
    char value;

    if (!dom_node_is_atom(dom)) {
        return NULL;
    }

    if (!parse_char(dom->atom, &value)) {
        return NULL;
    }

    return ast_make_literal_atomic_character(&dom->loc, value);
}

static struct AstNode *parse_literal_int(struct DomNode *dom)
{
    long value;

    if (!dom_node_is_atom(dom)) {
        return NULL;
    }

    if (!parse_int(dom->atom, &value)) {
        return NULL;
    }

    return ast_make_literal_atomic_int(&dom->loc, value);
}

static struct AstNode *parse_literal_real(struct DomNode *dom)
{
    double value;

    if (!dom_node_is_atom(dom)) {
        return NULL;
    }

    if (!parse_real(dom->atom, &value)) {
        return NULL;
    }

    return ast_make_literal_atomic_real(&dom->loc, value);
}

static struct AstNode *parse_literal(struct DomNode *dom)
{
    struct AstNode *result;

    if ((!err_state() && (result = parse_literal_unit(dom))) ||
        (!err_state() && (result = parse_literal_bool(dom))) ||
        (!err_state() && (result = parse_literal_char(dom))) ||
        (!err_state() && (result = parse_literal_real(dom))) ||
        (!err_state() && (result = parse_literal_int(dom))) ||
        (!err_state() && (result = parse_literal_string(dom)))) {
        return result;

    } else {
        return NULL;
    }
}

static struct AstNode *parse_reference(struct DomNode *dom)
{
    char *symbol;

    /* 1. Is a symbol. */
    if (!(symbol = dom_node_parse_symbol(dom))) {
        return NULL;
    }

    return ast_make_ctl_reference(&dom->loc, symbol);
}

static struct AstNode *parse_one(struct DomNode *dom)
{
    err_reset();
    struct AstNode *node;
    if ((!err_state() && (node = parse_literal(dom))) ||
        (!err_state() && (node = parse_reference(dom))) ||
        (!err_state() && (node = parse_do_block(dom))) ||
        (!err_state() && (node = parse_bind(dom))) ||
        (!err_state() && (node = parse_fdef(dom))) ||
        (!err_state() && (node = parse_match(dom))) ||
        (!err_state() && (node = parse_special(dom))) ||
        (!err_state() && (node = parse_fcall(dom))) ||
        (!err_state() && (node = parse_literal_compound(dom)))) {
        return node;

    } else {
        err_push_src("PARSE", dom->loc, "Failed parsing DOM node");
        return NULL;
    }
}

static struct AstNode *parse_list(struct DomNode *dom)
{
    struct AstNode *node;
    struct AstNode *result = NULL;
    struct AstNode *result_end = NULL;

    while (dom) {
        if ((node = parse_one(dom))) {
            LIST_APPEND(node, &result, &result_end);

        } else {
            ast_node_free(result);
            return NULL;
        }
        dom = dom->next;
    }

    return result;
}

struct AstNode *parse_source(char *source)
{
    struct DomNode *dom;
    struct AstNode *ast;

    dom = lex(source);
    if (err_state()) {
        err_push("PARSE", "Failed parsing source");
        return NULL;
    }

    ast = parse_list(dom);
    if (err_state()) {
        err_push("PARSE", "Failed parsing DOM list");
        dom_free(dom);
        return NULL;
    }

    dom_free(dom);
    return ast;
}
