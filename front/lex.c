/* Copyright (C) 2014-2016 Krzysztof Stachowiak */

#include <stdlib.h>
#include <ctype.h>

#include "log.h"
#include "collection.h"
#include "error.h"
#include "lex.h"

/* Tokenization.
 * =============
 */

static int tok_is_paren(int c)
{
    LOG_TRACE_FUNC
    return c == TOK_CORE_OPEN ||
           c == TOK_CORE_CLOSE ||
           c == TOK_ARR_OPEN ||
           c == TOK_ARR_CLOSE ||
           c == TOK_TUP_OPEN ||
           c == TOK_TUP_CLOSE;
}

static int tok_is_allowed_in_atom(int c)
{
    LOG_TRACE_FUNC
    return !tok_is_paren(c) &&
           !isspace(c) &&
           c != TOK_DELIM_STR &&
           c != TOK_DELIM_CHAR &&
           c != TOK_DELIM_ESCAPE &&
           c != TOK_COMMENT;
}


static struct SourceIter find_nonesc_delim(
        struct SourceIter *begin,
        struct SourceIter *end,
        int delim)
{
    struct SourceIter result = *begin;
    struct SourceIter prev;

    LOG_TRACE_FUNC

    if (result.current == result.first) {
        si_adv(&result);
    }

    prev = result;
    si_back(&prev);

    while (!si_eq(&result, end) &&
            (*(result.current) != delim ||
             *(prev.current) == TOK_DELIM_ESCAPE)) {
        si_adv(&result);
        si_adv(&prev);
    }

    return result;
}

static struct Token *tok_read_paren(
        struct SourceIter *current,
        struct SourceIter *end)
{
    struct Token *result;
    struct SourceIter prev;

    LOG_TRACE_FUNC

    if (si_eq(current, end) || !tok_is_paren(*(current->current))) {
        return NULL;
    }

    prev = *current;
    si_adv(current);
    result = tok_make_token(&prev, current);

    *current = si_find_if_not(current, end, isspace);

    return result;
}

static struct Token *tok_read_reg_atom(
        struct SourceIter *current,
        struct SourceIter *end)
{
    struct Token *result;
    struct SourceIter atom_end;

    LOG_TRACE_FUNC

    if (si_eq(current, end) || !tok_is_allowed_in_atom(*(current->current))) {
        return NULL;
    }

    atom_end = si_find_if_not(current, end, tok_is_allowed_in_atom);

    result = tok_make_token(current, &atom_end);
    *current = si_find_if_not(&atom_end, end, isspace);
    return result;
}

static struct Token *tok_read_delim_atom(
        struct SourceIter *current,
        struct SourceIter *end)
{
    struct Token *result;
    struct SourceIter atom_begin, atom_end;
    char delimiter;

    LOG_TRACE_FUNC

    atom_begin = *current;

    if (si_eq(current, end) || (
            *current->current != TOK_DELIM_STR &&
            *current->current != TOK_DELIM_CHAR)) {
        return NULL;
    }

    delimiter = *(current->current);

    si_adv(current);
    atom_end = find_nonesc_delim(current, end, delimiter);

    if (si_eq(&atom_end, end) || (*(atom_end.current) != delimiter)) {
        err_push_src("LEX", &atom_begin.loc,
                "undelimited %s",
                delimiter == TOK_DELIM_STR ? "string" : "character");
        return NULL;
    }

    si_adv(&atom_end);
    result = tok_make_token(&atom_begin, &atom_end);

    *current = si_find_if_not(&atom_end, end, isspace);

    return result;
}

static struct Token *tok_read_comment(
        struct SourceIter *current,
        struct SourceIter *end)
{
    struct Token *result;
    struct SourceIter comment_end;

    LOG_TRACE_FUNC

    if (si_eq(current, end) || *(current->current) != TOK_COMMENT) {
        return NULL;
    }

    comment_end = si_find(current, end, '\n');
    result = tok_make_token(current, &comment_end);

    if (!si_eq(&comment_end, end)) {
        si_adv(&comment_end);
        *current = si_find_if_not(&comment_end, end, isspace);
    } else {
        *current = *end;
    }

    return result;
}

static struct Token *tok_read_token(
        struct SourceIter *current,
        struct SourceIter *end)
{
    struct Token *result;

    LOG_TRACE_FUNC

    if ((!err_state() && (result = tok_read_paren(current, end))) ||
        (!err_state() && (result = tok_read_reg_atom(current, end))) ||
        (!err_state() && (result = tok_read_delim_atom(current, end))) ||
        (!err_state() && (result = tok_read_comment(current, end)))) {
        return result;
    } else {
        return NULL;
    }
}

static struct Token *tokenize(
        struct SourceIter begin,
        struct SourceIter end)
{
    struct SourceIter current = begin;

    struct Token *result = NULL;
    struct Token *result_end = NULL;

    LOG_TRACE_FUNC

    while (!si_eq(&current, &end)) {

        struct Token *tok = tok_read_token(&current, &end);

        if (!tok) {
            err_push_src(
		"LEX",
		&current.loc,
		"Failed reading token at %s",
		begin.first);
            tok_free(result);
            return NULL;
        }

        if (tok_is_comment(tok)) {
            tok_free(tok);
            LOG_TRACE("Skipping comment token");
        } else {
            LOG_TRACE("Read token from(%s) to (%s)", current.first, current.last);
            LIST_APPEND(tok, &result, &result_end);
        }
    }

    return result;
}

/* DOM building.
 * =============
 */

static enum DomCpdType dom_infer_cpd_type(struct Token *tok)
{
    if (tok_is_spec(tok, TOK_CORE_OPEN)) {
        return DOM_CPD_CORE;
    }

    if (tok_is_spec(tok, TOK_ARR_OPEN)) {
        return DOM_CPD_ARRAY;
    }

    if (tok_is_spec(tok, TOK_TUP_OPEN)) {
        return DOM_CPD_TUPLE;
    }

    LOG_ERROR("Logic corruption!");
    exit(1);
}

static struct DomNode *dom_parse_node(struct Token **current);

static struct DomNode *dom_parse_compound_node(struct Token **current)
{
    struct DomNode *children = NULL;
    struct DomNode *children_end = NULL;

    struct Token *first = *current;
    *current = (*current)->next;

    while (*current) {
        if (tok_is_close_paren(*current) && tok_paren_match(first, *current)) {
            struct DomNode *result = dom_make_compound(
                &first->loc, dom_infer_cpd_type(first), children);
            *current = (*current)->next;
            return result;

        } else {
            struct DomNode *child = dom_parse_node(current);
            if (err_state()) {
                goto fail;

            } else if (!child) {
                goto fail;

            } else {
                LIST_APPEND(child, &children, &children_end);
            }
        }
    }

    err_push_src(
	"LEX",
	&first->loc,
	"undelimited compound DOM node");

fail:
    dom_free(children);
    return NULL;
}

static struct DomNode *dom_parse_atom_node(struct Token **current)
{
    struct DomNode *result;
    result = dom_make_atom(&(*current)->loc, (*current)->begin, (*current)->end);
    *current = (*current)->next;
    return result;
}

static struct DomNode *dom_parse_node(struct Token **current)
{
    if (!(*current)) {
        return NULL;

    } else if (tok_is_close_paren(*current)) {
        err_push_src(
	    "LEX",
	    &(*current)->loc,
	    "closing unopened compound node");
        return NULL;

    } else if (tok_is_open_paren(*current)) {
        return dom_parse_compound_node(current);

    } else {
        return dom_parse_atom_node(current);

    }
}

static struct DomNode *dom_build(struct Token *tokens)
{
    struct Token *current = tokens;
    struct DomNode *result = NULL;
    struct DomNode *result_end = NULL;

    while (current) {
        struct DomNode *node = dom_parse_node(&current);
        if (err_state()) {
            dom_free(result);
            return NULL;
        }
        if (node) {
            LOG_TRACE(
                "parsed sexpr node \"%s\"",
                node->type == DOM_ATOM ? "atom" : "comopund");
            LIST_APPEND(node, &result, &result_end);
        }
    }

    return result;
}

/* Public API.
 * ===========
 */

struct DomNode *lex(char *source)
{
    struct SourceIter begin, end;
    struct Token *tokens;
    struct DomNode *dom;

    int len = strlen(source);
    char *source_end = source + len;

    si_init(&begin, source, source_end);

    si_init(&end, source, source_end);
    si_advn(&end, len);

    if (!(tokens = tokenize(begin, end))) {
        return NULL;
    }

    if (!(dom = dom_build(tokens))) {
        tok_free(tokens);
        return NULL;
    }

    tok_free(tokens);
    return dom;
}

