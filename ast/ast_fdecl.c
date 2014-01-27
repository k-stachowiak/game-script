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
#include <ctype.h>

#include "common/logg.h"
#include "ast/ast.h"

static bool ast_is_symbol(char *string)
{
        int i;

        if (isdigit(string[0]))
                return false;

        for (i = 0; i < AST_KEYWORD_COUNT; ++i)
                if (ast_is_keyword(string, i))
                        return false;

        return true;
}

static bool ast_parse_symbol_list(struct dom_node *node,
                                  char ***form_args,
                                  int *form_args_count)
{
        struct dom_node *children;
        int children_count;
        int i;

        if (node->type != DOM_COMPOUND ||
            node->body.compound.type != DOM_CPD_CORE)
                return false;

        children = node->body.compound.children;
        children_count = node->body.compound.children_count;

        *form_args = calloc(children_count, sizeof(**form_args));

        for (i = 0; i < children_count; ++i) {

                int symbol_len;

                if (children[i].type != DOM_ATOM ||
                    !ast_is_symbol(children[i].body.atom.string))
                        goto error;

                symbol_len = strlen(children[i].body.atom.string);
                (*form_args)[i] = malloc(symbol_len + 1);
                strcpy((*form_args)[i], children[i].body.atom.string);
        }

        return true;

error:
        for (i = 0; i < children_count; ++i)
                if ((*form_args)[i])
                        free((*form_args)[i]);

        free(*form_args);

        return false;
}

struct ast_func_decl *ast_parse_func_decl(struct dom_node *node)
{
        struct dom_node *children;
        int children_count;

        char *symbol;
        char **form_args;
        int form_args_count;
        struct ast_node *first_expr;
        struct ast_node *current_expr;

        struct ast_func_decl *result;

        int i;
        int symbol_len;

        char *log_buffer = NULL;

        symbol = NULL;
        form_args = NULL;
        form_args_count = 0;
        first_expr = NULL;

        current_expr = NULL;

        // Validate basic format.
        if (node->type != DOM_COMPOUND &&
            node->body.compound.type != DOM_CPD_CORE)
                goto error;

        children = node->body.compound.children;
        children_count = node->body.compound.children_count;

        if (children_count < 3)
                goto error;

        if (children[0].type != DOM_ATOM ||
            !ast_is_keyword(children[0].body.atom.string, AST_KEYWORD_FUNC))
                goto error;

        // Read symbol.
        if (children[1].type != DOM_ATOM)
                goto error;

        symbol_len = strlen(children[1].body.atom.string);
        symbol = malloc(symbol_len + 1);
        strcpy(symbol, children[1].body.atom.string);

        // Read formal arguments.
        if (children[2].type != DOM_COMPOUND ||
            children[2].body.compound.type != DOM_CPD_CORE ||
            !ast_parse_symbol_list(children + 2, &form_args, &form_args_count))
                goto error;

        // Read expressions.
        for (i = 3; i < children_count; ++i) {
                struct ast_node *expr;
                expr = ast_parse_expression(children + i);
                if (expr)
                        ast_push(expr, &first_expr, &current_expr);
                else
                        goto error;
        }

        log_buffer = dom_print(node);
        LOG_TRACE("Parsing func declaration [SUCCESS]:\n%s", log_buffer);
        free(log_buffer);

        result = malloc(sizeof(*result));
        result->symbol = symbol;
        result->form_args = form_args;
        result->form_args_count = form_args_count;
        result->first_expr = first_expr;

        return result;

error:
        log_buffer = dom_print(node);
        LOG_TRACE("Parsing func declaration [FAILURE]:\n%s", log_buffer);
        free(log_buffer);

        if (symbol)
                free(symbol);

        if (form_args_count > 0) {
                free(form_args);
                for (i = 0; i < form_args_count; ++i)
                        free(form_args[i]);
        }

        if (first_expr)
                ast_delete_node(first_expr);

        return NULL;
}

void ast_delete_func_decl(struct ast_func_decl *fd)
{
        int i;

        if (fd->symbol) {
                free(fd->symbol);
                fd->symbol = NULL;
        }

        if (fd->form_args_count) {
                for (i = 0; i < fd->form_args_count; ++i)
                        free(fd->form_args[i]);
                free(fd->form_args);
                fd->form_args = NULL;
                fd->form_args_count = 0;
        }

        if (fd->first_expr) {
                ast_delete_node(fd->first_expr);
                fd->first_expr = NULL;
        }
}

