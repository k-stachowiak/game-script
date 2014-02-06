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

#include <string.h>
#include <stdlib.h>

#include "common/logg.h"
#include "ast/ast.h"

struct ast_unit *ast_parse_unit(struct dom_node *node)
{
        struct dom_node *dom_children;
        int dom_children_count;

        char *name;

        struct ast_node *functions;
        int functions_count;
        int functions_cap;

        struct ast_unit *result;

        int name_len;
        int i;

        name = NULL;
        functions = NULL;
        functions_count = 0;
        functions_cap = 0;

        result = NULL;

        // Validate node.
        if (node->type != DOM_COMPOUND ||
            node->body.compound.type != DOM_CPD_CORE) {
                LOG_TRACE("DOM node not core compound.");
                goto error;
        }

        dom_children = node->body.compound.children;
        dom_children_count = node->body.compound.children_count;

        if (dom_children_count < 2) {
                LOG_TRACE("Incorrect numger of node children :%d.", dom_children_count);
                goto error;
        }

        if (dom_children[0].type != DOM_ATOM ||
            !ast_is_keyword(dom_children[0].body.atom.string, AST_KEYWORD_MODULE)) {
                LOG_TRACE("Moudle keyword not found, \"%s\" encountered instead.",
                        dom_children[0].body.atom.string);
                goto error;
        }

        if (dom_children[1].type != DOM_ATOM) {
                LOG_TRACE("Unit name not found.");
                goto error;
        }

        // Parse node.
        name_len = strlen(dom_children[1].body.atom.string);
        name = malloc(name_len + 1);
        strcpy(name, dom_children[1].body.atom.string);

        LOG_TRACE("Parsed unit name \"%s\".", name);

        for (i = 2; i < dom_children_count; ++i) {

                bool alloc_success;
                struct ast_node *node;
                node = ast_parse_expression(dom_children + i);

                if (node == NULL || node->type != AST_FUNC_DECL)
                        goto error;

                alloc_success = ast_push(node, &functions, &functions_count, &functions_cap);
                free(node);

                if (!alloc_success)
                        goto error;
        }

        result = malloc(sizeof(*result));
        result->name = name;
        result->functions = functions;
        result->functions_count = functions_count;

        return result;

error:
        if (name) {
                free(name);
        }

        if (functions) {
                for (i = 0; i < functions_count; ++i) {
                        ast_delete_node(functions + i);
                }
                free(functions);
        }

        return NULL;
}

void ast_delete_unit(struct ast_unit *unit)
{
        int i;

        for (i = 0; i < unit->functions_count; ++i) {
                ast_delete_node(unit->functions + i);
        }

        if (i > 0) {
                free(unit->functions);
        }
}
