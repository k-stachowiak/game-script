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
        struct dom_node *dom_children;
        int dom_children_count;

        char *symbol;
        struct ast_node *act_args;
        int act_args_count;
        int act_args_cap;

        struct ast_func_call *result;

        int symbol_len;
        int i;

        char *log_buffer;

        symbol = NULL;
        act_args = NULL;
        act_args_count = 0;
        act_args_cap = 0;
        result = NULL;
        log_buffer = NULL;

        // Validate basic format.
        if (node->type != DOM_COMPOUND ||
            node->body.compound.type != DOM_CPD_CORE)
                goto error;

        dom_children = node->body.compound.children;
        dom_children_count = node->body.compound.children_count;

        if (dom_children_count == 0)
                goto error;

        // Read symbol.
        if (dom_children[0].type != DOM_ATOM ||
            !ast_is_symbol(dom_children[0].body.atom.string))
                goto error;

        symbol_len = strlen(dom_children[0].body.atom.string);
        symbol = malloc(symbol_len + 1);
        strcpy(symbol, dom_children[0].body.atom.string);

        // Read the arguments.
        for (i = 1; i < dom_children_count; ++i) {
                struct ast_node *act_arg;
                act_arg = ast_parse_expression(dom_children + i);
                if (!act_arg)
                        goto error;

                ast_push(act_arg, &act_args, &act_args_count, &act_args_cap);
        }

        log_buffer = dom_print(node);
        LOG_TRACE("Parsing func call [SUCCESS]:\n%s", log_buffer);
        free(log_buffer);

        // Build the result.
        result = malloc(sizeof(*result));
        result->symbol = symbol;
        result->act_args = act_args;
        result->act_args_count = act_args_count;

        return result;

error:
        log_buffer = dom_print(node);
        LOG_TRACE("Parsing func call [FAILURE]:\n%s", log_buffer);
        free(log_buffer);

        if (symbol)
                free(symbol);

        if (act_args) {
                for (i = 0; i < act_args_count; ++i) {
                        ast_delete_node(act_args + i);
                }
                free(act_args);
        }

        return NULL;
}

void ast_delete_func_call(struct ast_func_call *fc)
{
        int i;

        if (fc->symbol) {
                free(fc->symbol);
                fc->symbol = NULL;
        }

        for (i = 0; i < fc->act_args_count; ++i) {
                ast_delete_node(fc->act_args + i);
        }

        if (i > 0) {
                free(fc->act_args);
        }
}

