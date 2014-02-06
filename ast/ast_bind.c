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

struct ast_bind *ast_parse_bind(struct dom_node *node)
{
        struct dom_node *dom_children;
        int dom_children_count;

        char *symbol;
        int symbol_len;
        struct ast_node *expr;

        struct ast_bind *result;

        char *log_buffer;

        symbol = NULL;
        expr = NULL;

        // Validate basic format.
        if (node->type != DOM_COMPOUND ||
            node->body.compound.type != DOM_CPD_CORE)
                goto error;

        dom_children = node->body.compound.children;
        dom_children_cound = node->body.compound.children_count;

        if (dom_children_count != 3)
                goto error;

        if (dom_children[0].type != DOM_ATOM ||
            !ast_is_keyword(dom_children[0].body.atom.string, AST_KEYWORD_BIND))
                goto error;

        // Read symbol.
        if (dom_children[1].type != DOM_ATOM)
                goto error;

        symbol_len = strlen(dom_children[1].body.atom.string);
        symbol = malloc(symbol_len + 1);
        strcpy(symbol, dom_children[1].body.atom.string);

        // Read expression.
        expr = ast_parse_expression(dom_children + 2);
        if (!expr)
                goto error;

        log_buffer = dom_print_node(node);
        LOG_TRACE("Parsing bind [SUCCESS]:\n%s", log_buffer);
        free(log_buffer);

        result = malloc(sizeof(*result));
        result->symbol = symbol;
        result->expr = expr;

        return result;

error:
        if (symbol) {
                free(symbol);
        }

        if (expr) {
                ast_delete_node(expr);
                free(expr);
        }

        return NULL;
}

void ast_delete_bind(struct ast_bind *b)
{
        if (b->symbol) {
                free(b->symbol);
        }

        if (b->expr) {
                ast_delete_node(b->expr);
                free(b->expr);
        }
}
