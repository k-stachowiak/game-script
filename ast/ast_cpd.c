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

struct ast_compound *ast_parse_compound(struct dom_node *node)
{
        struct dom_node *dom_children;
        int dom_children_count;

        struct ast_node *ast_children;
        int ast_children_count;
        int ast_children_cap;

        struct ast_compound *result;

        int i;

        char *log_buffer;

        dom_children = NULL;
        dom_children_count = 0;
        ast_children = NULL;
        ast_children_count = 0;
        ast_children_cap = 0;
        result = NULL;
        log_buffer = NULL;

        // Validate node format.
        if (node->type != DOM_COMPOUND ||
           (node->type == DOM_COMPOUND &&
            node->body.compound.type == DOM_CPD_CORE)) {
                goto error;
        }

        dom_children = node->body.compound.children;
        dom_children_count = node->body.compound.children_count;

        // Read the elements.
        for (i = 0; i < dom_children_count; ++i) {
                struct ast_node *child;
                child = ast_parse_expression(dom_children + i);
                if (!child || !ast_push(child,
                                        &ast_children,
                                        &ast_children_count,
                                        &ast_children_cap)) {
                        goto error;
                }
        }

        result = malloc(sizeof(*result));
        result->children = ast_children;
        result->children_count = ast_children_count;

        switch (node->body.compound.type) {
        case DOM_CPD_LIST:
                log_buffer = dom_print(node);
                LOG_TRACE("Parsing compound list [SUCCESS]:\n%s", log_buffer);
                free(log_buffer);
                result->type = AST_CPD_LIST;
                break;
        case DOM_CPD_ARRAY:
                log_buffer = dom_print(node);
                LOG_TRACE("Parsing compound array [SUCCESS]:\n%s", log_buffer);
                free(log_buffer);
                result->type = AST_CPD_ARRAY;
                break;
        case DOM_CPD_TUPLE:
                log_buffer = dom_print(node);
                LOG_TRACE("Parsing compound tuple [SUCCESS]:\n%s", log_buffer);
                free(log_buffer);
                result->type = AST_CPD_TUPLE;
                break;
        case DOM_CPD_CORE:
                LOG_ERROR("Control should never reach this point.");
                goto error;
        }

        return result;

error:
        log_buffer = dom_print(node);
        LOG_TRACE("Parsing compound [FAILURE]:\n%s", log_buffer);
        free(log_buffer);

        if (ast_children) {
                for (i = 0; i < ast_children_count; ++i) {
                        ast_delete_node(ast_children + i);
                }
                free(ast_children);
        }

        return NULL;
}

void ast_delete_compound(struct ast_compound *cpd)
{
        int i;

        for (i = 0; i < cpd->children_count; ++i) {
                ast_delete_node(cpd->children + i);
        }

        if (i > 0) {
                free(cpd->children);
        }
}

