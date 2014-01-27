/* Copyright (C) 2013 Krzysztof Stachowiak */

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

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "common/logg.h"
#include "sexpr/tok.h"

// Token recognition.
// ------------------

static inline bool is_any_paren(int c)
{
        return c == TOK_CORE_OPEN ||
               c == TOK_CORE_CLOSE ||
               c == TOK_LIST_OPEN ||
               c == TOK_LIST_CLOSE ||
               c == TOK_ARR_OPEN ||
               c == TOK_ARR_CLOSE ||
               c == TOK_TUP_OPEN ||
               c == TOK_TUP_CLOSE;
}

static inline bool allowed_in_atom(int c)
{
        return !is_any_paren(c) &&
               !isspace(c) &&
             c != TOK_STR_DELIM &&
             c != TOK_DELIM_ESCAPE &&
             c != TOK_COMMENT;
}

// Seek algorithms.
// ----------------

static char *skip_spaces(char *current)
{
        while (*current && isspace(*current)) ++current;
        return current;
}

static char *find_endl(char *current)
{
        while (*current && *current != '\n') ++current;
        return current;
}

static char *find_nonescaped_strdelim(char *current, char delim)
{
        while (*current && (*current != TOK_STR_DELIM ||
                           (*current == TOK_STR_DELIM &&
                           *(current - 1) == TOK_DELIM_ESCAPE))) {
                ++current;
        }
        return current;
}

static char *find_not_allowed(char *current)
{
        while (*current && allowed_in_atom(*current))
                ++current;
        return current;
}

// Dynamic memory management.
// --------------------------

static void tok_push(char *begin, char *end,
                     struct tok_def **tok_defs,
                     int *tok_count,
                     int *tok_cap)
{
        char *token;

        // Adjust capacity if needed.
        if (*tok_cap == 0) {
                *tok_count = 0;
                *tok_cap = 10;
                *tok_defs = malloc(*tok_cap * sizeof(**tok_defs));

        } else if (*tok_cap == *tok_count) {
                *tok_cap *= 2;
                *tok_defs = realloc(*tok_defs, *tok_cap * sizeof(**tok_defs));
        }

        // Perform the insertion.
        ((*tok_defs)[*tok_count]).begin = begin;
        ((*tok_defs)[*tok_count]).end = end;
        ++(*tok_count);

        token = token_to_string((*tok_defs) + *tok_count - 1);
        free(token);
}

// Tokenizer cases logic.
// ----------------------

static char *tok_parenthesis(char *current,
                             struct tok_def **tok_defs,
                             int *tok_count,
                             int *tok_cap)
{
        if (!(*current) || !is_any_paren(*current))
                return current;

        tok_push(current, current + 1, tok_defs, tok_count, tok_cap);

        return skip_spaces(current + 1);
}

static char *tok_comments(char *current,
                struct tok_def **tok_defs,
                int *tok_count,
                int *tok_cap)
{
        if (!(*current) || *current != TOK_COMMENT)
                return current;

        current = find_endl(current + 1);

        return skip_spaces(current + 1);
}

static char *tok_regular_atom(char *current,
                struct tok_def **tok_defs,
                int *tok_count,
                int *tok_cap)
{
        char *atom_end;

        if (!(*current) || !allowed_in_atom(*current))
                return current;

        atom_end = find_not_allowed(current);

        tok_push(current, atom_end, tok_defs, tok_count, tok_cap);

        return skip_spaces(atom_end);
}

static char *tok_delim_atom(char *current,
                            struct tok_def **tok_defs,
                            int *tok_count,
                            int *tok_cap,
                            bool *error_flag)
{
        char delim;
        char *string_end;

        *error_flag = false;

        if (!(*current) || (*current != TOK_STR_DELIM &&
                            *current != TOK_CHAR_DELIM))
                return current;

        delim = *current;
        string_end = find_nonescaped_strdelim(current + 1, delim);

        if (*string_end != delim) {
                LOG_DEBUG("Unescaped string literal detected while parsing.");
                *error_flag = true;
                return NULL;
        }

        tok_push(current, string_end + 1, tok_defs, tok_count, tok_cap);

        return skip_spaces(string_end + 1);
}

// Tokenizer API implementation.
// -----------------------------

bool tokenize(char *source, struct tok_def **tok_defs, int *tok_count)
{
        int tok_cap;
        char *current;
        bool error_flag;

        tok_cap = 0;
        current = skip_spaces(source);
        error_flag = false;

        *tok_defs = NULL;
        *tok_count = 0;

        while (*current) {
                current = tok_parenthesis(current, tok_defs, tok_count, &tok_cap);
                current = tok_comments(current, tok_defs, tok_count, &tok_cap);
                current = tok_regular_atom(current, tok_defs, tok_count, &tok_cap);
                current = tok_delim_atom(current, tok_defs, tok_count, &tok_cap, &error_flag);
                if (error_flag)
                        goto error;
        }

        return true;

error:
        if (*tok_defs)
                free(*tok_defs);
        *tok_defs = NULL;
        *tok_count = 0;
        return false;
}

char *token_to_string(struct tok_def *token)
{
        ptrdiff_t token_len;
        char *result;

        token_len = token->end - token->begin;
        result = malloc(token_len + 1);
        memcpy(result, token->begin, token_len);
        result[token_len] = '\0';
        return result;
}

bool token_is_char(struct tok_def *token, int c)
{
        if (token->end - token->begin != 1)
                return false;

        return *(token->begin) == c;
}

bool token_is_closing_paren(struct tok_def *token)
{
        return token_is_char(token, TOK_CORE_CLOSE) ||
               token_is_char(token, TOK_LIST_CLOSE) ||
               token_is_char(token, TOK_ARR_CLOSE) ||
               token_is_char(token, TOK_TUP_CLOSE);
}

bool token_paren_match(struct tok_def *lhs, struct tok_def *rhs)
{
        return (token_is_char(lhs, TOK_CORE_OPEN) && token_is_char(rhs, TOK_CORE_CLOSE)) ||
               (token_is_char(lhs, TOK_LIST_OPEN) && token_is_char(rhs, TOK_LIST_CLOSE)) ||
               (token_is_char(lhs, TOK_ARR_OPEN) && token_is_char(rhs, TOK_ARR_CLOSE)) ||
               (token_is_char(lhs, TOK_TUP_OPEN) && token_is_char(rhs, TOK_TUP_CLOSE));
}

bool token_delim_match(struct tok_def *lhs, struct tok_def *rhs)
{
        return (token_is_char(lhs, TOK_STR_DELIM) && token_is_char(rhs, TOK_STR_DELIM)) ||
               (token_is_char(lhs, TOK_CHAR_DELIM) && token_is_char(rhs, TOK_CHAR_DELIM));
}
