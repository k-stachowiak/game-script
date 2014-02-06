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

#include <stdlib.h>
#include <string.h>

#include "common/logg.h"
#include "ast/ast.h"

static bool ast_parse_lit_boolean(char *atom, int *boolean)
{
        if (ast_is_keyword(atom, AST_KEYWORD_TRUE)) {
                *boolean = 1;
                return true;
        }

        if (ast_is_keyword(atom, AST_KEYWORD_FALSE)) {
                *boolean = 0;
                return true;
        }

        return false;
}

static bool ast_parse_lit_string(char *atom, char **string)
{
        int len;
        len = strlen(atom);

        if (atom[0] == TOK_STR_DELIM && atom[len - 1] == TOK_STR_DELIM) {
                *string = malloc(len - 2 + 1);
                memcpy(*string, atom + 1, len - 2);
                (*string)[len - 2] = '\0';
                return true;
        }

        return false;
}

static bool ast_parse_lit_character(char *atom, char *character)
{
        int len;
        len = strlen(atom);

        if (len == 3 &&
            atom[0] == TOK_CHAR_DELIM &&
            atom[len - 1] == TOK_CHAR_DELIM) {
                *character = atom[1];
                return true;
        }

        return false;
}

static bool ast_parse_lit_integer(char *atom, long *integer)
{
        int len;
        char *end;

        len = strlen(atom);
        *integer = strtol(atom, &end, 10);

        return end == (atom + len);
}

static bool ast_parse_lit_real(char *atom, double *real)
{
        int len;
        char *end;

        len = strlen(atom);
        *real = strtod(atom, &end);

        return end == (atom + len);
}

struct ast_literal *ast_parse_literal(struct dom_node *node)
{
        char *atom;

        struct ast_literal *result;

        long integer;
        double real;
        char character;
        char *string;
        int boolean;

        char *log_buffer = NULL;

        if (node->type != DOM_ATOM)
                return NULL;

        atom = node->body.atom.string;

        if (ast_parse_lit_boolean(atom, &boolean)) {
                result = malloc(sizeof(*result));
                result->type = AST_LIT_BOOLEAN;
                result->body.boolean = boolean;
                goto success;
        }

        if (ast_parse_lit_string(atom, &string)) {
                result = malloc(sizeof(*result));
                result->type = AST_LIT_STRING;
                result->body.string = string;
                goto success;
        }

        if (ast_parse_lit_character(atom, &character)) {
                result = malloc(sizeof(*result));
                result->type = AST_LIT_CHARACTER;
                result->body.character = character;
                goto success;
        }

        if (ast_parse_lit_integer(atom, &integer)) {
                result = malloc(sizeof(*result));
                result->type = AST_LIT_INTEGER;
                result->body.integer = integer;
                goto success;
        }

        if (ast_parse_lit_real(atom, &real)) {
                result = malloc(sizeof(*result));
                result->type = AST_LIT_REAL;
                result->body.real = real;
                goto success;
        }

        log_buffer = dom_print(node);
        LOG_TRACE("Parsing literal [FAILURE]:\n%s", log_buffer);
        free(log_buffer);

        return NULL;

success:
        log_buffer = dom_print(node);
        LOG_TRACE("Parsing literal [SUCCESS]:\n%s", log_buffer);
        free(log_buffer);

        return result;
}

void ast_delete_literal(struct ast_literal *lit)
{
        if (lit->type == AST_LIT_STRING) {
                free(lit->body.string);
        }
}

