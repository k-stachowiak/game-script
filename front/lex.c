/* Copyright (C) 2014,2015 Krzysztof Stachowiak */

#include <stdlib.h>
#include <ctype.h>

#include "log.h"
#include "collection.h"
#include "error.h"
#include "lex.h"

/* Error reporting.
 * ================
 */

static void lex_error_undelimited(char *what, struct SourceLocation *where)
{
    struct ErrMessage msg;
    err_msg_init_src(&msg, "lex", where);
    err_msg_append(&msg, "undelimited %s", what);
    err_msg_set(&msg);
	err_push("LEX", *where, "undelimited %s", what);
}

static void lex_error_close_unopen(struct SourceLocation *where)
{
    struct ErrMessage msg;
    err_msg_init_src(&msg, "lex", where);
    err_msg_append(&msg, "closing unopened compound node");
    err_msg_set(&msg);
	err_push("LEX", *where, "closing unopened compound node");
}

static void lex_error_read(char *what, struct SourceLocation *where)
{
    struct ErrMessage msg;
    err_msg_init_src(&msg, "LEX", where);
    err_msg_append(&msg, "Failed reading %s", what);
    err_msg_set(&msg);
	err_push("LEX", *where, "Failed reading %s", what);
}

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

static struct SourceIter find(
        struct SourceIter *begin,
        struct SourceIter *end,
        int value)
{
    struct SourceIter result = *begin;

    LOG_TRACE_FUNC

    while (!si_eq(&result, end) && *(result.current) != value) {
        si_adv(&result);
    }

    return result;
}

static struct SourceIter find_if_not(
        struct SourceIter *begin,
        struct SourceIter *end,
        int (*pred)(int))
{
    struct SourceIter result = *begin;

    LOG_TRACE_FUNC

    while (!si_eq(&result, end) && pred(*(result.current))) {
        si_adv(&result);
    }

    return result;
}

static struct Token *find_non_comment(struct Token *token)
{
	while (token && tok_is_comment(token)) {
		token = token->next;
	}
	return token;
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

    *current = find_if_not(current, end, isspace);

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

    atom_end = find_if_not(current, end, tok_is_allowed_in_atom);

    result = tok_make_token(current, &atom_end);
    *current = find_if_not(&atom_end, end, isspace);
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
        lex_error_undelimited(
            delimiter == TOK_DELIM_STR ? "string" : "character",
            &atom_begin.loc);
        return NULL;
    }

    si_adv(&atom_end);
    result = tok_make_token(&atom_begin, &atom_end);

    *current = find_if_not(&atom_end, end, isspace);

    return result;
}

static struct Token *tok_read_comment(
        struct SourceIter *current,
        struct SourceIter *end)
{
    /* Note that the comments are to be discarded, however in order to
     * maintain a consistent algorithm's structure they will be treated
     * like other tokens at this stage. They must be filtered out at
     * a later stage.
     */

    struct Token *result;
    struct SourceIter comment_end;

    LOG_TRACE_FUNC

    if (si_eq(current, end) || *(current->current) != TOK_COMMENT) {
        return NULL;
    }

    comment_end = find(current, end, '\n');
    result = tok_make_token(current, &comment_end);

    if (!si_eq(&comment_end, end)) {
        si_adv(&comment_end);
        *current = find_if_not(&comment_end, end, isspace);
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
		err_push("LEX", current->loc, "Failed reading token at %s", current->first);
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

    err_reset();
    while (!si_eq(&current, &end)) {

        struct Token *tok = tok_read_token(&current, &end);

        if (!tok) {
            if (!err_state()) {
                lex_error_read("token", &begin.loc);
            }
			err_push("LEX", current.loc, "Failed tokenizing at %s", current.first);
            tok_free(result);
            return NULL;
        }

        LOG_TRACE("Read token from(%s) to (%s)", current.first, current.last);
        LIST_APPEND(tok, &result, &result_end);
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
    *current = find_non_comment((*current)->next);

    while (*current) {
        if (tok_is_close_paren(*current) && tok_paren_match(first, *current)) {
            struct DomNode *result = dom_make_compound(
                &first->loc, dom_infer_cpd_type(first), children);
            *current = find_non_comment((*current)->next);
            return result;

        } else {
            struct DomNode *child = dom_parse_node(current);
            if (err_state()) {
				err_push("LEX", (*current)->loc, "Failed parsing compound node: %s", (*current)->begin);
                goto fail;

            } else if (!child) {
                lex_error_read("compound DOM node", &(*current)->loc);
                goto fail;

            } else {
                LIST_APPEND(child, &children, &children_end);
            }
        }
    }

    lex_error_undelimited("compound DOM node", &first->loc);

fail:
    dom_free(children);
    return NULL;
}

static struct DomNode *dom_parse_atom_node(struct Token **current)
{
    struct DomNode *result;
    result = dom_make_atom(&(*current)->loc, (*current)->begin, (*current)->end);
    *current = find_non_comment((*current)->next);
    return result;
}

/**
 * Parses a DOM node starting from the current token.
 * NOTE: this will silently skip all the tokens
 * starting with the comment character.
 */
static struct DomNode *dom_parse_node(struct Token **current)
{
    *current = find_non_comment(*current);

    if (!(*current)) {
        return NULL;

    } else if (tok_is_close_paren(*current)) {
        lex_error_close_unopen(&(*current)->loc);
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

    err_reset();
    while (current) {
		struct SourceLocation current_loc = current->loc;
        struct DomNode *node = dom_parse_node(&current);
        if (err_state()) {
			err_push("LEX", current_loc, "Failed building DOM");
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

