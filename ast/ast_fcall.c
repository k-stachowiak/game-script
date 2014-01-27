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

struct ast_func_call *ast_parse_func_call(struct dom_node *node)
{
        struct dom_node *children;
        int children_count;

        char *symbol;
        struct ast_node *first_act_arg;
        struct ast_node *current_act_arg;

        struct ast_func_call *result;

        int symbol_len;
        int i;

        char *log_buffer = NULL;

        symbol = NULL;
        first_act_arg = NULL;
        current_act_arg = NULL;
        result = NULL;

        // Validate basic format.
        if (node->type != DOM_COMPOUND &&
            node->body.compound.type != DOM_CPD_CORE)
                goto error;

        children = node->body.compound.children;
        children_count = node->body.compound.children_count;

        if (children_count == 0)
                goto error;

        // Read symbol.
        if (children[0].type != DOM_ATOM)
                goto error;

        symbol_len = strlen(children[0].body.atom.string);
        symbol = malloc(symbol_len + 1);
        strcpy(symbol, children[0].body.atom.string);

        // Read the arguments.
        for (i = 1; i < children_count; ++i) {
                struct ast_node *act_arg;
                act_arg = ast_parse_expression(children + i);
                if (!act_arg)
                        goto error;

                ast_push(act_arg, &first_act_arg, &current_act_arg);
        }

        log_buffer = dom_print(node);
        LOG_TRACE("Parsing func call [SUCCESS]:\n%s", log_buffer);
        free(log_buffer);

        // Build the result.
        result = malloc(sizeof(*result));
        result->symbol = symbol;
        result->first_act_arg = first_act_arg;

        return result;

error:
        log_buffer = dom_print(node);
        LOG_TRACE("Parsing func call [FAILURE]:\n%s", log_buffer);
        free(log_buffer);

        if (symbol)
                free(symbol);

        if (first_act_arg)
                ast_delete_node(first_act_arg);

        return NULL;
}

void ast_delete_func_call(struct ast_func_call *fc)
{
        if (fc->symbol) {
                free(fc->symbol);
                fc->symbol = NULL;
        }

        if (fc->first_act_arg) {
                ast_delete_node(fc->first_act_arg);
                fc->first_act_arg = NULL;
        }
}

