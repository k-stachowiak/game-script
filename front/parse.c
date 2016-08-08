/* Copyright (C) 2014-2016 Krzysztof Stachowiak */

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
#include "ast_loc_map.h"

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

/* Parser state definition.
 * ========================
 */

struct ParserState {
    void *data;
    ParserAstCallback acb;
};

/* Parsing algorithm.
 * ==================
 */

static struct AstNode *parse_one(struct DomNode *dom, struct ParserState *state);
static struct AstNode *parse_list(struct DomNode *dom, struct ParserState *state);

static struct AstNode *parse_symbol(
        struct DomNode *dom,
        struct ParserState *state)
{
    char *symbol;
    struct AstNode *result;

    (void)state;

    /* 1. Is a symbol. */
    if (!(symbol = dom_node_parse_symbol(dom))) {
        return NULL;
    }

    result = ast_make_symbol(symbol);
    mem_free(symbol);

    return result;
}

static struct AstNode *parse_literal_void(struct DomNode *dom)
{
    if (dom_node_is_spec_reserved_atom(dom, DOM_RES_VOID)) {
        return ast_make_literal_atomic_unit();
    } else {
        return NULL;
    }
}

static struct AstNode *parse_literal_unit(struct DomNode *dom)
{
    if (dom_node_is_spec_reserved_atom(dom, DOM_RES_UNIT)) {
        return ast_make_literal_atomic_unit();
    } else {
        return NULL;
    }
}

static struct AstNode *parse_literal_bool(struct DomNode *dom)
{
    if (dom_node_is_spec_reserved_atom(dom, DOM_RES_TRUE)) {
        return ast_make_literal_atomic_bool(1);
    } else if (dom_node_is_spec_reserved_atom(dom, DOM_RES_FALSE)) {
        return ast_make_literal_atomic_bool(0);
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

    result = ast_make_literal_atomic_string(value);
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

    return ast_make_literal_atomic_character(value);
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

    return ast_make_literal_atomic_int(value);
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

    return ast_make_literal_atomic_real(value);
}

static struct AstNode *parse_literal_datatype(struct DomNode *dom)
{
    if (dom_node_is_spec_reserved_atom(dom, DOM_RES_TVOID)) {
        return ast_make_literal_atomic_datatype(AST_LIT_ATOM_DATATYPE_VOID);
    } else if (dom_node_is_spec_reserved_atom(dom, DOM_RES_TUNIT)) {
        return ast_make_literal_atomic_datatype(AST_LIT_ATOM_DATATYPE_UNIT);
    } else if (dom_node_is_spec_reserved_atom(dom, DOM_RES_BOOLEAN)) {
        return ast_make_literal_atomic_datatype(AST_LIT_ATOM_DATATYPE_BOOLEAN);
    } else if (dom_node_is_spec_reserved_atom(dom, DOM_RES_INTEGER)) {
        return ast_make_literal_atomic_datatype(AST_LIT_ATOM_DATATYPE_INTEGER);
    } else if (dom_node_is_spec_reserved_atom(dom, DOM_RES_REAL)) {
        return ast_make_literal_atomic_datatype(AST_LIT_ATOM_DATATYPE_REAL);
    } else if (dom_node_is_spec_reserved_atom(dom, DOM_RES_CHARACTER)) {
        return ast_make_literal_atomic_datatype(AST_LIT_ATOM_DATATYPE_CHARACTER);
    } else {
        return NULL;
    }
}

static struct AstNode *parse_literal_atomic(
        struct DomNode *dom,
        struct ParserState *state)
{
    struct AstNode *result;

    (void)state;

    if ((!err_state() && (result = parse_literal_void(dom))) ||
        (!err_state() && (result = parse_literal_unit(dom))) ||
        (!err_state() && (result = parse_literal_bool(dom))) ||
        (!err_state() && (result = parse_literal_char(dom))) ||
        (!err_state() && (result = parse_literal_real(dom))) ||
        (!err_state() && (result = parse_literal_int(dom))) ||
        (!err_state() && (result = parse_literal_string(dom))) ||
        (!err_state() && (result = parse_literal_datatype(dom)))) {
        return result;

    } else {
        return NULL;
    }
}

static struct AstNode *parse_do_block(
        struct DomNode *dom,
        struct ParserState *state)
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
    if (!(exprs = parse_list(child, state))) {
        return NULL;
    }

    return ast_make_spec_do(exprs);
}

static struct AstNode *parse_match(
        struct DomNode *dom,
        struct ParserState *state)
{
    struct DomNode *child = NULL;
    struct AstNode *expr = NULL;
    struct AstNode *keys = NULL, *keys_end = NULL;
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
    if (!(expr = parse_one(child, state))) {
        return NULL;
    }
    child = child->next;

    /* 3.3. Has at least one matching expression. */
    while (child) {
        struct DomNode *match_child = NULL;
        struct AstNode *key = NULL;
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
        if (!(key = parse_one(match_child, state))) {
            goto fail;
        }
        match_child = match_child->next;

        /* 3.3.4. 2nd child is an expression. */
        if (!(value = parse_one(match_child, state))) {
            goto fail;
        }

        LIST_APPEND(key, &keys, &keys_end);
        LIST_APPEND(value, &values, &values_end);

        child = child->next;
    }

    return ast_make_spec_match(expr, keys, values);

fail:
    ast_node_free(expr);

    if (keys) {
        ast_node_free(keys);
    }

    if (values) {
        ast_node_free(values);
    }

    return NULL;
}

static struct AstNode *parse_if(
        struct DomNode *dom,
        struct ParserState *state)
{
    struct DomNode *child = NULL;
    struct AstNode *test = NULL;
    struct AstNode *true_expr = NULL;
    struct AstNode *false_expr = NULL;

    /* 1. Is compound CORE */
    if (!dom_node_is_spec_compound(dom, DOM_CPD_CORE)) {
        return NULL;
    }

    /* 2. Has 4 children */
    if (!dom_node_is_cpd_of_size(dom, 4)) {
        return NULL;
    }

    child = dom->cpd_children;

    /* 3.1. 1st child is the if keyword. */
    if (!dom_node_is_spec_reserved_atom(child, DOM_RES_IF)) {
        return NULL;
    }
    child = child->next;

    /* 3.2 2nd child is a valid expression. */
    if (!(test = parse_one(child, state))) {
        return NULL;
    }
    child = child->next;

    /* 3.3 3rd child is a valid expression. */
    if (!(true_expr = parse_one(child, state))) {
        ast_node_free(test);
        return NULL;
    }
    child = child->next;

    /* 3.4 4th child is a valid expression. */
    if (!(false_expr = parse_one(child, state))) {
        ast_node_free(test);
        ast_node_free(true_expr);
        return NULL;
    }

    return ast_make_spec_if(test, true_expr, false_expr);
}

static struct AstNode *parse_while(
        struct DomNode *dom,
        struct ParserState *state)
{
    struct DomNode *child = NULL;
    struct AstNode *test = NULL;
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

    /* 3.1. 1st child is the while keyword. */
    if (!dom_node_is_spec_reserved_atom(child, DOM_RES_WHILE)) {
        return NULL;
    }
    child = child->next;

    /* 3.2 2nd child is a valid expression. */
    if (!(test = parse_one(child, state))) {
        return NULL;
    }
    child = child->next;

    /* 3.3 3rd child is a valid expression. */
    if (!(expr = parse_one(child, state))) {
        ast_node_free(test);
        return NULL;
    }

    return ast_make_spec_while(test, expr);
}

static struct AstNode *parse_func_def(
        struct DomNode *dom,
        struct ParserState *state)
{
    struct AstNode *expr = NULL;

    struct DomNode *child = NULL;
    struct DomNode *arg_child = NULL;

    struct AstNode *formal_args = NULL, *formal_args_end = NULL;

    /* 1. Is compound CORE. */
    if (!dom_node_is_spec_compound(dom, DOM_CPD_CORE)) {
        return NULL;
    }

    /* 2. Has 3 children. */
    if (!dom_node_is_cpd_of_size(dom, 3)) {
        return NULL;
    }

    child = dom->cpd_children;

    /* 2.1. 1st child is "func" keyword. */
    if (!dom_node_is_spec_reserved_atom(child, DOM_RES_FUNC)) {
        return NULL;
    }
    child = child->next;

    /* 2.2. 2nd keyword is a core compound of patterns. */
    if (!dom_node_is_spec_compound(child, DOM_CPD_CORE)) {
        return NULL;
    }

    arg_child = child->cpd_children;

    /* Argument list may be empty. */
    if (arg_child) {
        while (arg_child) {
            struct AstNode *pattern;
            if (!(pattern = parse_one(arg_child, state))) {
                goto fail;
            } else {
                LIST_APPEND(pattern, &formal_args, &formal_args_end);
            }
            arg_child = arg_child->next;
        }
    }

    child = child->next;

    /* 2.3. Has 1 more further expression. */
    if (!(expr = parse_one(child, state))) {
        goto fail;
    }

    return ast_make_spec_func_def(formal_args, expr);

fail:
    if (formal_args) {
        ast_node_free(formal_args);
    }

    return NULL;
}

static struct AstNode *parse_tagged_type(
        struct DomNode *dom,
        struct ParserState *state)
{
    struct DomNode *child = NULL;
    char *tag = NULL;
    struct AstNode *type = NULL;

    struct AstNode *result;

    /* 1. Is compound CORE */
    if (!dom_node_is_spec_compound(dom, DOM_CPD_CORE)) {
        return NULL;
    }

    /* 2. Has 3 children */
    if (!dom_node_is_cpd_of_size(dom, 3)) {
        return NULL;
    }

    child = dom->cpd_children;

    /* 2.1. 1st child is tagged-type keyword. */
    if (!dom_node_is_spec_reserved_atom(child, DOM_RES_TAGGED_TYPE)) {
        return NULL;
    }
    child = child->next;

    /* 2.2. 2nd child is tag. */
    if (!(tag = dom_node_parse_symbol(dom))) {
        return NULL;
    }
    child = child->next;

    /* 2.3 3rd child is type expression. */
    if (!(type = parse_one(child, state))) {
        mem_free(tag);
        return NULL;
    }

    result = ast_make_spec_tagged_type(tag, type);
    mem_free(tag);

    return result;
}

static struct AstNode *parse_bind(
        struct DomNode *dom,
        struct ParserState *state)
{
    struct DomNode *child = NULL;
    struct AstNode *pattern = NULL;
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

    /* 2.1. 1st child is bind keyword. */
    if (!dom_node_is_spec_reserved_atom(child, DOM_RES_BIND)) {
        return NULL;
    }
    child = child->next;

    /* 2.2. 2nd child is pattern. */
    if (!(pattern = parse_one(child, state))) {
        return NULL;
    }
    child = child->next;

    /* 2.3 3rd child is any expression. */
    if (!(expr = parse_one(child, state))) {
        ast_node_free(pattern);
        return NULL;
    }

    return ast_make_spec_bind(pattern, expr);
}

static struct AstNode *parse_min_nary(
        struct DomNode *dom,
        int min_args,
        enum Reserved keyword,
        struct AstNode *(*constructor)(
            struct AstNode *args),
        struct ParserState *state)
{
    struct DomNode *child = NULL;
    struct AstNode *exprs = NULL;

    /* 1. Is compound CORE. */
    if (!dom_node_is_spec_compound(dom, DOM_CPD_CORE)) {
        return NULL;
    }

    /* 2. Has a keyword and min or more children. */
    if (!dom_node_is_cpd_min_size(dom, 1 + min_args)) {
        return NULL;
    }

    child = dom->cpd_children;

    /* 2.1. 1st child is a proper keyword. */
    if (!dom_node_is_spec_reserved_atom(child, keyword)) {
        return NULL;
    }
    child = child->next;

    /* 2.2. Has proper amount of further children being any expression. */
    exprs = parse_list(child, state);
    if (err_state() || !exprs || ast_list_len(exprs) < min_args) {
        return NULL;
    }

    return constructor(exprs);
}

static struct AstNode *parse_unary(
        struct DomNode *dom,
        enum Reserved keyword,
        struct AstNode *(*constructor)(
            struct AstNode *),
        struct ParserState *state)
{
    struct DomNode *child = NULL;
    struct AstNode *arg = NULL;

    /* 1. Is compound CORE */
    if (!dom_node_is_spec_compound(dom, DOM_CPD_CORE)) {
        return NULL;
    }

    /* 2. Has 2 children */
    if (!dom_node_is_cpd_of_size(dom, 2)) {
        return NULL;
    }

    child = dom->cpd_children;

    /* 2.1. 1st child is the proper keyword. */
    if (!dom_node_is_spec_reserved_atom(child, keyword)) {
        return NULL;
    }
    child = child->next;

    /* 2.2 2nd child is a valid expression. */
    if (!(arg = parse_one(child, state))) {
        return NULL;
    }
    child = child->next;

    return constructor(arg);
}

static struct AstNode *parse_binary(
        struct DomNode *dom,
        enum Reserved keyword,
        struct AstNode *(*constructor)(
            struct AstNode *,
            struct AstNode *),
        struct ParserState *state)
{
    struct DomNode *child = NULL;
    struct AstNode *arg1 = NULL;
    struct AstNode *arg2 = NULL;

    /* 1. Is compound CORE */
    if (!dom_node_is_spec_compound(dom, DOM_CPD_CORE)) {
        return NULL;
    }

    /* 2. Has 2 children */
    if (!dom_node_is_cpd_of_size(dom, 3)) {
        return NULL;
    }

    child = dom->cpd_children;

    /* 2.1. 1st child is the proper keyword. */
    if (!dom_node_is_spec_reserved_atom(child, keyword)) {
        return NULL;
    }
    child = child->next;

    /* 2.2 2nd child is a valid expression. */
    if (!(arg1 = parse_one(child, state))) {
        return NULL;
    }
    child = child->next;

    /* 2.3 3rd child is a valid expression. */
    if (!(arg2 = parse_one(child, state))) {
        ast_node_free(arg1);
        return NULL;
    }
    child = child->next;

    return constructor(arg1, arg2);
}

static struct AstNode *parse_special(
        struct DomNode *dom,
        struct ParserState *state)
{
    struct AstNode *result;

    if ((!err_state() && (result = parse_do_block(dom, state))) ||
        (!err_state() && (result = parse_match(dom, state))) ||
        (!err_state() && (result = parse_if(dom, state))) ||
        (!err_state() && (result = parse_while(dom, state))) ||
        (!err_state() && (result = parse_func_def(dom, state))) ||
        (!err_state() && (result = parse_min_nary(dom, 1, DOM_RES_AND, ast_make_spec_bool_and, state))) ||
        (!err_state() && (result = parse_min_nary(dom, 1, DOM_RES_OR, ast_make_spec_bool_or, state))) ||
        (!err_state() && (result = parse_min_nary(dom, 1, DOM_RES_SET_OF, ast_make_spec_set_of, state))) ||
        (!err_state() && (result = parse_unary(dom, DOM_RES_RANGE_OF, ast_make_spec_range_of, state))) ||
        (!err_state() && (result = parse_unary(dom, DOM_RES_ARRAY_OF, ast_make_spec_array_of, state))) ||
        (!err_state() && (result = parse_min_nary(dom, 1, DOM_RES_TUPLE_OF, ast_make_spec_tuple_of, state))) ||
        (!err_state() && (result = parse_unary(dom, DOM_RES_POINTER_TO, ast_make_spec_pointer_to, state))) ||
        (!err_state() && (result = parse_min_nary(dom, 1, DOM_RES_FUNCTION, ast_make_spec_function_type, state))) ||
        (!err_state() && (result = parse_unary(dom, DOM_RES_TYPE_PRODUCT, ast_make_spec_type_product, state))) ||
        (!err_state() && (result = parse_unary(dom, DOM_RES_TYPE_UNION, ast_make_spec_type_union, state))) ||
        (!err_state() && (result = parse_tagged_type(dom, state))) ||
        (!err_state() && (result = parse_bind(dom, state))) ||
        (!err_state() && (result = parse_unary(dom, DOM_RES_REF, ast_make_spec_ref, state))) ||
        (!err_state() && (result = parse_unary(dom, DOM_RES_PEEK, ast_make_spec_peek, state))) ||
        (!err_state() && (result = parse_binary(dom, DOM_RES_POKE, ast_make_spec_poke, state))) ||
        (!err_state() && (result = parse_unary(dom, DOM_RES_BEGIN, ast_make_spec_begin, state))) ||
        (!err_state() && (result = parse_unary(dom, DOM_RES_END, ast_make_spec_end, state))) ||
        (!err_state() && (result = parse_unary(dom, DOM_RES_INC, ast_make_spec_inc, state))) ||
        (!err_state() && (result = parse_unary(dom, DOM_RES_SUCC, ast_make_spec_succ, state)))) {
        return result;

    } else {
        return NULL;
    }
}

static struct AstNode *parse_func_call(
        struct DomNode *dom,
        struct ParserState *state)
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
    func = parse_one(child, state);
    if (err_state()) {
        return NULL;
    }
    child = child->next;

    /* 3.2. Has 0 or more further children being any expression. */
    args = parse_list(child, state);
    if (err_state()) {
        return NULL;
    }

    return ast_make_func_call(func, args);
}

static struct AstNode *parse_literal_compound(
        struct DomNode *dom,
        struct ParserState *state)
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
    exprs = parse_list(dom->cpd_children, state);
    if (err_state()) {
        return NULL;
    } else {
        return ast_make_literal_compound(type, exprs);
    }
}

static struct AstNode *parse_one(
        struct DomNode *dom,
        struct ParserState *state)
{
    err_reset();
    struct AstNode *node;
    if ((!err_state() && (node = parse_literal_atomic(dom, state))) ||
        (!err_state() && (node = parse_symbol(dom, state))) ||
        (!err_state() && (node = parse_special(dom, state))) ||
        (!err_state() && (node = parse_func_call(dom, state))) ||
        (!err_state() && (node = parse_literal_compound(dom, state)))) {
        if (state->acb) {
            state->acb(state->data, node, &dom->loc);
        }
        return node;

    } else {
        err_push_src(
            "PARSE",
            &dom->loc,
            "Failed parsing DOM node");
        return NULL;
    }
}

static struct AstNode *parse_list(
        struct DomNode *dom,
        struct ParserState *state)
{
    struct AstNode *node;
    struct AstNode *result = NULL;
    struct AstNode *result_end = NULL;

    while (dom) {
        if ((node = parse_one(dom, state))) {
            LIST_APPEND(node, &result, &result_end);

        } else {
            ast_node_free(result);
            return NULL;
        }
        dom = dom->next;
    }

    return result;
}

struct AstNode *parse_source(
        char *source,
        void *data,
        ParserAstCallback acb)
{
    struct DomNode *dom;
    struct AstNode *ast;

    struct ParserState state = { data, acb };

    dom = lex(source);
    if (err_state()) {
        err_push("PARSE", "Failed parsing source");
        return NULL;
    }

    ast = parse_list(dom, &state);
    if (err_state()) {
        err_push("PARSE", "Failed parsing DOM list");
        dom_free(dom);
        return NULL;
    }

    dom_free(dom);
    return ast;
}

void parse_source_on_ast(
        void *data,
        struct AstNode *node,
        struct SourceLocation *loc)
{
    struct AstLocMap *alm = (struct AstLocMap *)data;
    alm_put(alm, node, loc);
}

struct AstNode *parse_source_build_alm(
        char *source,
        struct AstLocMap *alm)
{
    return parse_source(source, alm, parse_source_on_ast);
}

