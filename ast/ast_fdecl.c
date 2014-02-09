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

static bool ast_parse_symbol_list(struct dom_node *node,
                                  char ***form_args,
                                  int *form_args_count)
{
        struct dom_node *dom_children;
        int dom_children_count;
        int i;

        if (node->type != DOM_COMPOUND ||
            node->body.compound.type != DOM_CPD_CORE)
                return false;

        dom_children = node->body.compound.children;
        dom_children_count = node->body.compound.children_count;

        *form_args = calloc(dom_children_count, sizeof(**form_args));

        for (i = 0; i < dom_children_count; ++i) {

                int symbol_len;

                if (dom_children[i].type != DOM_ATOM ||
                    !ast_is_symbol(dom_children[i].body.atom.string))
                        goto error;

                symbol_len = strlen(dom_children[i].body.atom.string);
                (*form_args)[i] = malloc(symbol_len + 1);
                strcpy((*form_args)[i], dom_children[i].body.atom.string);
        }

        return true;

error:
        for (i = 0; i < dom_children_count; ++i)
                if ((*form_args)[i])
                        free((*form_args)[i]);

        free(*form_args);

        return false;
}

struct ast_func_decl *ast_parse_func_decl(struct dom_node *node)
{
        struct dom_node *dom_children;
        int dom_children_count;

        char *symbol;
        int symbol_len;

        char **form_args;
        int form_args_count;

        struct ast_node *exprs;
        int exprs_count;
        int exprs_cap;

        struct ast_func_decl *result;

        int i;

        char *log_buffer = NULL;

        symbol = NULL;
        form_args = NULL;
        form_args_count = 0;
        exprs = NULL;
        exprs_count = 0;
        exprs_cap = 0;

        // Validate basic format.
        if (node->type != DOM_COMPOUND &&
            node->body.compound.type != DOM_CPD_CORE)
                goto error;

        dom_children = node->body.compound.children;
        dom_children_count = node->body.compound.children_count;

        if (dom_children_count < 3)
                goto error;

        if (dom_children[0].type != DOM_ATOM ||
            !ast_is_keyword(dom_children[0].body.atom.string, AST_KEYWORD_FUNC))
                goto error;

        // Read symbol.
        if (dom_children[1].type != DOM_ATOM)
                goto error;

        symbol_len = strlen(dom_children[1].body.atom.string);
        symbol = malloc(symbol_len + 1);
        strcpy(symbol, dom_children[1].body.atom.string);

        // Read formal arguments.
        if (dom_children[2].type != DOM_COMPOUND ||
            dom_children[2].body.compound.type != DOM_CPD_CORE ||
            !ast_parse_symbol_list(dom_children + 2, &form_args, &form_args_count))
                goto error;

        // Read exprs.
        for (i = 3; i < dom_children_count; ++i) {
                struct ast_node *expr;
                expr = ast_parse_expression(dom_children + i);
                if (expr || !ast_push(expr, &exprs, &exprs_count, &exprs_cap)) {
                        goto error;
                }
        }

        log_buffer = dom_print(node);
        LOG_TRACE("Parsing func declaration [SUCCESS]:\n%s", log_buffer);
        free(log_buffer);

        result = malloc(sizeof(*result));
        result->symbol = symbol;
        result->form_args = form_args;
        result->form_args_count = form_args_count;
        result->exprs = exprs;
        result->exprs_count = exprs_count;

        return result;

error:
        log_buffer = dom_print(node);
        LOG_TRACE("Parsing func declaration [FAILURE]:\n%s", log_buffer);
        free(log_buffer);

        if (symbol) {
                free(symbol);
        }

        if (form_args_count > 0) {
                for (i = 0; i < form_args_count; ++i) {
                        free(form_args[i]);
                }
                free(form_args);
        }

        if (exprs) {
                for (i = 0; i < exprs_count; ++i) {
                        ast_delete_node(exprs + i);
                }
                free(exprs);
        }

        return NULL;
}

void ast_delete_func_decl(struct ast_func_decl *fd)
{
        int i;

        if (!fd) {
                return;
        }

        if (fd->symbol) {
                free(fd->symbol);
        }

        if (fd->form_args_count) {
                for (i = 0; i < fd->form_args_count; ++i)
                        free(fd->form_args[i]);
                free(fd->form_args);
        }

        for (i = 0; i < fd->exprs_count; ++i) {
                ast_delete_node(fd->exprs + i);
        }

        if (i > 0) {
                free(fd->exprs);
        }
}

