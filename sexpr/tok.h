/* Copyright (C) 2014 Krzysztof Stachowiak */

/*
 * This file is part of moon.
 *
 * moon is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * moon is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with moon. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TOK_H
#define TOK_H

#include <stdbool.h>

enum tok_char {
        TOK_CORE_OPEN = '(',
        TOK_CORE_CLOSE = ')',
        TOK_LIST_OPEN = '{',
        TOK_LIST_CLOSE = '}',
        TOK_ARR_OPEN = '[',
        TOK_ARR_CLOSE = ']',
        TOK_TUP_OPEN = '<',
        TOK_TUP_CLOSE = '>',
        TOK_COMMENT = '#',
        TOK_STR_DELIM = '"',
        TOK_CHAR_DELIM = '\'',
        TOK_DELIM_ESCAPE = '\\'
};

struct tok_def {
        char *begin;
        char *end;
};

bool tokenize(char *source, struct tok_def **tok_defs, int *tok_count);

char *token_to_string(struct tok_def *token);
bool token_is_char(struct tok_def *token, int c);
bool token_is_closing_paren(struct tok_def *token);
bool token_paren_match(struct tok_def *lhs, struct tok_def *rhs);
bool token_delim_match(struct tok_def *lhs, struct tok_def *rhs);

#endif
