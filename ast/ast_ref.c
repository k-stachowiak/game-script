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

struct ast_reference *ast_parse_reference(struct dom_node *node)
{
        struct ast_reference *result;
        int symbol_len;
        char *log_buffer;

        result = NULL;
        log_buffer = NULL;

        // Validate node format.
        if (node->type != DOM_ATOM ||
            !ast_is_symbol(node->body.atom.string)) {
                log_buffer = dom_print(node);
                LOG_TRACE("Parsing reference [FAILURE]:\n%s", log_buffer);
                free(log_buffer);
                return NULL;
        }

        symbol_len = strlen(node->body.atom.string);

        result = malloc(sizeof(*result));
        result->symbol = malloc(symbol_len + 1);
        strcpy(result->symbol, node->body.atom.string);

        log_buffer = dom_print(node);
        LOG_TRACE("Parsing reference [SUCCESS]:\n%s", log_buffer);
        free(log_buffer);

        return result;
}

void ast_delete_reference(struct ast_reference *ref)
{
        if (!ref) {
                return;
        }

        if (ref->symbol) {
                free(ref->symbol);
        }
}
