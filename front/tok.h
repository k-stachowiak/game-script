/* Copyright (C) 2014,2015 Krzysztof Stachowiak */

#ifndef TOK_H
#define TOK_H

#include <stdbool.h>

#include "src_iter.h"

struct Token {
    /* Data */
    struct SourceLocation loc;
    char *begin, *end;

    /* Intrusive list */
    struct Token *next;
};

enum TokenType {
    TOK_CORE_OPEN = '(',
    TOK_CORE_CLOSE = ')',
    TOK_ARR_OPEN = '[',
    TOK_ARR_CLOSE = ']',
    TOK_TUP_OPEN = '{',
    TOK_TUP_CLOSE = '}',
    TOK_COMMENT = '#',
    TOK_DELIM_STR = '"',
    TOK_DELIM_CHAR = '\'',
    TOK_DELIM_ESCAPE = '\\'
};

/* Creation.
 * =========
 */

struct Token *tok_make_token(struct SourceIter *begin, struct SourceIter *end);

/* Destruction.
 * ============
 */

void tok_free(struct Token* tokens);

/* Operations.
 * ===========
 */

int tok_list_length(struct Token* tokens);
bool tok_is_spec(struct Token *tok, enum TokenType type);
bool tok_is_open_paren(struct Token *tok);
bool tok_is_close_paren(struct Token *tok);
bool tok_is_comment(struct Token *tok);
bool tok_paren_match(struct Token *lhs, struct Token *rhs);

#endif
