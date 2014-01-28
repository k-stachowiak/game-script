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
        struct dom_node *children;
        int children_count;

        struct ast_node *first_element;
        struct ast_node *current_element;

        struct ast_compound *result;

        int i;

        char *log_buffer;

        first_element = NULL;
        current_element = NULL;
        result = NULL;
        log_buffer = NULL;

        // Validate node format.
        if (node->type != DOM_COMPOUND ||
           (node->type == DOM_COMPOUND &&
            node->body.compound.type == DOM_CPD_CORE)) {
                goto error;
        }

        children = node->body.compound.children;
        children_count = node->body.compound.children_count;

        // Read the elements.
        for (i = 0; i < children_count; ++i) {
                struct ast_node *element;
                element = ast_parse_expression(children + i);
                if (!element)
                        goto error;

                ast_push(element, &first_element, &current_element);
        }

        result = malloc(sizeof(*result));
        result->first_element = first_element;

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

        if (first_element)
                ast_delete_node(first_element);

        return NULL;
}

void ast_delete_compound(struct ast_compound *cpd)
{
        if (cpd->first_element) {
                ast_delete_node(cpd->first_element);
                cpd->first_element = NULL;
        }
}

