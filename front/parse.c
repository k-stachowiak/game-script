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

static struct Pattern *parse_pattern(struct DomNode *dom)
{
    char *symbol = dom_node_parse_symbol(dom);
    struct Pattern *children;

    if (symbol) {
        return pattern_make_symbol(symbol);

    } else if (dom_node_is_reserved_atom(dom, DOM_RES_DONTCARE)) {
        return pattern_make_dontcare();

    } else if (dom->type == DOM_ATOM) {
		err_push_src("PARSE", dom->loc, "Attempt at binding to a literal");
        return NULL;

    } else {
        switch (dom->cpd_type) {
        case DOM_CPD_CORE:
			err_push_src("PARSE", dom->loc, "Core compound encountered where a pattern was expected");
            return NULL;

        case DOM_CPD_ARRAY:
            if (!(children = parse_pattern_list(dom->cpd_children))) {
                if (!err_state()) {
					err_push_src("PARSE", dom->loc, "Empty compound pattern encountered");
                }
                return NULL;

            } else {
                return pattern_make_compound(children, PATTERN_ARRAY);
            }

        case DOM_CPD_TUPLE:
            if (!(children = parse_pattern_list(dom->cpd_children))) {
                if (!err_state()) {
					err_push_src("PARSE", dom->loc, "Empty compound pattern encountered");
                }
                return NULL;

            } else {
                return pattern_make_compound(children, PATTERN_TUPLE);
            }
        }
    }

    LOG_ERROR("Impossible to get here.");
    exit(1);
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

    LOG_TRACE_FUNC

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
    if (!dom_node_is_reserved_atom(child, DOM_RES_DO)) {
        return NULL;
    }
    child = child->next;

    /* 3.2. Has 1 or more further expressions. */
    if (!(exprs = parse_list(child))) {
        return NULL;
    }

    return ast_make_do_block(&dom->loc, exprs);
}

static struct AstNode *parse_bind(struct DomNode *dom)
{
    struct DomNode *child = NULL;
    struct Pattern *pattern = NULL;
    struct AstNode *expr = NULL;

    LOG_TRACE_FUNC

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
    if (!dom_node_is_reserved_atom(child, DOM_RES_BIND)) {
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

    return ast_make_bind(&dom->loc, pattern, expr);
}

static struct AstNode *parse_parafunc(struct DomNode *dom)
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
    if (dom_node_is_reserved_atom(child, DOM_RES_AND)) {
        return ast_make_parafunc(&dom->loc, AST_PARAFUNC_AND, args);
    }

    /* 3.2. Case ||: */
    if (dom_node_is_reserved_atom(child, DOM_RES_OR)) {
        return ast_make_parafunc(&dom->loc, AST_PARAFUNC_OR, args);
    }

    /* 3.3. Case if: */
    if (dom_node_is_reserved_atom(child, DOM_RES_IF)) {
        return ast_make_parafunc(&dom->loc, AST_PARAFUNC_IF, args);
    }

    /* 3.4. Case switch: */
    if (dom_node_is_reserved_atom(child, DOM_RES_SWITCH)) {
        return ast_make_parafunc(&dom->loc, AST_PARAFUNC_SWITCH, args);
    }

    /* 3.5. Case ref: */
    if (dom_node_is_reserved_atom(child, DOM_RES_REF)) {
        return ast_make_parafunc(&dom->loc, AST_PARAFUNC_REF, args);
    }

    /* 3.6. Case peek: */
    if (dom_node_is_reserved_atom(child, DOM_RES_PEEK)) {
        return ast_make_parafunc(&dom->loc, AST_PARAFUNC_PEEK, args);
    }

    /* 3.7. Case poke: */
    if (dom_node_is_reserved_atom(child, DOM_RES_POKE)) {
        return ast_make_parafunc(&dom->loc, AST_PARAFUNC_POKE, args);
    }

    /* 3.8. Case begin: */
    if (dom_node_is_reserved_atom(child, DOM_RES_BEGIN)) {
        return ast_make_parafunc(&dom->loc, AST_PARAFUNC_BEGIN, args);
    }

    /* 3.9. Case end: */
    if (dom_node_is_reserved_atom(child, DOM_RES_END)) {
        return ast_make_parafunc(&dom->loc, AST_PARAFUNC_END, args);
    }

    /* 3.10. Case succ: */
    if (dom_node_is_reserved_atom(child, DOM_RES_SUCC)) {
        return ast_make_parafunc(&dom->loc, AST_PARAFUNC_SUCC, args);
    }

    /* None of the reserved words were matched. */
	ast_node_free(args);
    return NULL;
}

static struct AstNode *parse_compound(struct DomNode *dom)
{
    enum AstCompoundType type;
    struct AstNode *exprs = NULL;

    LOG_TRACE_FUNC

    /* 1. Is compound. */
    if (dom_node_is_atom(dom)) {
        return NULL;
    }

    /* 2. Is of ARRAY or TUPLE type. */
    switch (dom->cpd_type) {
    case DOM_CPD_ARRAY:
        type = AST_CPD_ARRAY;
        break;

    case DOM_CPD_TUPLE:
        type = AST_CPD_TUPLE;
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
        return ast_make_compound(&dom->loc, type, exprs);
    }
}

static struct AstNode *parse_func_call(struct DomNode *dom)
{
    struct AstNode *func = NULL;
    struct AstNode *args = NULL;
    struct DomNode *child = NULL;

    LOG_TRACE_FUNC

    /* 1. Is compound CORE. */
    if (!dom_node_is_spec_compound(dom, DOM_CPD_CORE)) {
        return NULL;
    }

    /* 2. Has 1 or more children. */
    if (!dom_node_is_cpd_min_size(dom, 1)) {
        return NULL;
    }

    child = dom->cpd_children;

    /* 3.1. 1st child is an expression. */
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

	return ast_make_func_call(&dom->loc, func, args);
}

static struct AstNode *parse_func_def(struct DomNode *dom)
{
    struct {
        struct SourceLocation *data;
        int cap, size;
    } arg_locs = { NULL, 0, 0 };

    int arg_count = 0;
    struct AstNode *expr = NULL;

    struct DomNode *child = NULL;
    struct DomNode *arg_child = NULL;

    struct Pattern *formal_args = NULL, *formal_args_end = NULL;

    LOG_TRACE_FUNC

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
    if (!dom_node_is_reserved_atom(child, DOM_RES_FUNC)) {
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

    arg_count = arg_locs.size;
    return ast_make_func_def(
        &dom->loc,
        formal_args,
        mem_realloc(
            arg_locs.data,
            arg_locs.size * sizeof(*arg_locs.data)),
        arg_count,
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

static struct AstNode *parse_literal_unit(struct DomNode *dom)
{
    LOG_TRACE_FUNC
    if (dom_node_is_reserved_atom(dom, DOM_RES_UNIT)) {
        return ast_make_literal_unit(&dom->loc);
    } else {
        return NULL;
    }
}

static struct AstNode *parse_literal_bool(struct DomNode *dom)
{
    LOG_TRACE_FUNC
    if (dom_node_is_reserved_atom(dom, DOM_RES_TRUE)) {
        return ast_make_literal_bool(&dom->loc, 1);
    } else if (dom_node_is_reserved_atom(dom, DOM_RES_FALSE)) {
        return ast_make_literal_bool(&dom->loc, 0);
    } else {
        return NULL;
    }
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

static struct AstNode *parse_literal_string(struct DomNode *dom)
{
    char *atom;
    int len;
    char delim = TOK_DELIM_STR;

    LOG_TRACE_FUNC

    if (!dom_node_is_atom(dom)) {
        return NULL;
    }

    atom = dom->atom;
    len = strlen(atom);

    if (atom[0] == delim && atom[len - 1] == delim && len >= 2) {
        char *unescaped = unescape_string(atom);
        if (!unescaped) {
            return NULL;
        } else {
            return ast_make_literal_string(&dom->loc, unescaped);
        }
    } else {
        return NULL;
    }
}

static struct AstNode *parse_literal_char(struct DomNode *dom)
{
    char *atom;
    int len;
    char delim = TOK_DELIM_CHAR;

    LOG_TRACE_FUNC

    if (!dom_node_is_atom(dom)) {
        return NULL;
    }

    atom = dom->atom;
    len = strlen(atom);

    if (atom[0] != delim || atom[len - 1] != delim) {
        return NULL;
    }

    if (len == 3) {
        return ast_make_literal_character(&dom->loc, atom[1]);

    } else if (len == 4 && atom[1] == TOK_DELIM_ESCAPE) {
        switch(atom[2]) {
         case 'a':
            return ast_make_literal_character(&dom->loc, '\a');
         case 'b':
            return ast_make_literal_character(&dom->loc, '\b');
         case 't':
            return ast_make_literal_character(&dom->loc, '\t');
         case 'n':
            return ast_make_literal_character(&dom->loc, '\n');
         case 'f':
            return ast_make_literal_character(&dom->loc, '\f');
         case 'r':
            return ast_make_literal_character(&dom->loc, '\r');
         case 'v':
            return ast_make_literal_character(&dom->loc, '\v');
         case '\\':
            return ast_make_literal_character(&dom->loc, '\\');
         case '\'':
            return ast_make_literal_character(&dom->loc, '\'');
         case '0':
            return ast_make_literal_character(&dom->loc, '\0');
         default:
            return NULL;
        }

    } else {
		err_push_src("PARSE", dom->loc, "Incorrect character length (%d)", len);
        return NULL;
    }
}

static struct AstNode *parse_literal_int(struct DomNode *dom)
{
    char *atom;
    int len;

    LOG_TRACE_FUNC

    if (!dom_node_is_atom(dom)) {
        return NULL;
    }

    atom = dom->atom;
    len = strlen(atom);

    if (len > 0) {
        char *first = atom, *last = atom + len;
        if (atom[0] == '-' || atom[0] == '+') {
            ++first;
        }

        if (first == last) {
			return NULL;
        }

        if (all_of(first, last, isdigit)) {
            long value = atol(atom);
            return ast_make_literal_int(&dom->loc, value);
        }
    }

    return NULL;
}

static struct AstNode *parse_literal_real(struct DomNode *dom)
{
    char *atom, *first, *period, *last;
    double value;
    int len;

    LOG_TRACE_FUNC

    if (!dom_node_is_atom(dom)) {
        return NULL;
    }

    atom = dom->atom;
    len = strlen(atom);

    first = atom;
    last = first + len;
    period = find(first, last, '.');

    if (atom[0] == '-' || atom[0] == '+') {
        ++first;
    }

    if (first == last) {
		return NULL;
    }

    if (period == last ||
        !all_of(first, period, isdigit) ||
        !all_of(period + 1, last, isdigit)) {
        return NULL;
    }

    value = atof(atom);
    return ast_make_literal_real(&dom->loc, value);
}

static struct AstNode *parse_literal(struct DomNode *dom)
{
    struct AstNode *result;

    LOG_TRACE_FUNC

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

    LOG_TRACE_FUNC

    /* 1. Is a symbol. */
    if (!(symbol = dom_node_parse_symbol(dom))) {
        return NULL;
    }

    return ast_make_reference(&dom->loc, symbol);
}

static struct AstNode *parse_one(struct DomNode *dom)
{
    err_reset();
    struct AstNode *node;
    if ((!err_state() && (node = parse_literal(dom))) ||
        (!err_state() && (node = parse_reference(dom))) ||
        (!err_state() && (node = parse_do_block(dom))) ||
        (!err_state() && (node = parse_bind(dom))) ||
        (!err_state() && (node = parse_func_def(dom))) ||
        (!err_state() && (node = parse_parafunc(dom))) ||
        (!err_state() && (node = parse_func_call(dom))) ||
        (!err_state() && (node = parse_compound(dom)))) {
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

