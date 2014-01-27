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

bool ast_is_keyword(char *string, enum ast_keyword kw)
{
        static char *kw_map[] = {
                "module",
                "func",
                "true",
                "false"
        };

        return strcmp(string, kw_map[kw]) == 0;
}

void ast_push(struct ast_node *node,
              struct ast_node **begin,
              struct ast_node **end)
{
        if (*begin == NULL) {
                *begin = node;
                *end = node;
                return;
        }

        (*end)->next = node;
        (*end) = node;
}

struct ast_node *ast_parse_expression(struct dom_node *node)
{
        struct ast_node *result;

        struct ast_func_decl *fd;
        struct ast_func_call *fc;
        struct ast_literal *lit;

        if ((fd = ast_parse_func_decl(node))) {
                result = malloc(sizeof(*result));
                result->next = NULL;
                result->type = AST_FUNC_DECL;
                memcpy(&(result->body.func_decl), fd, sizeof(*fd));
                free(fd);
                return result;
        }

        if ((fc = ast_parse_func_call(node))) {
                result = malloc(sizeof(*result));
                result->next = NULL;
                result->type = AST_FUNC_CALL;
                memcpy(&(result->body.func_call), fc, sizeof(*fc));
                free(fc);
                return result;
        }

        if ((lit = ast_parse_literal(node))) {
                result = malloc(sizeof(*result));
                result->next = NULL;
                result->type = AST_LITERAL;
                memcpy(&(result->body.literal), lit, sizeof(*lit));
                free(lit);
                return result;
        }

        return NULL;
}

void ast_delete_node(struct ast_node *node)
{
        switch (node->type) {
        case AST_FUNC_DECL:
                ast_delete_func_decl(&(node->body.func_decl));
                break;
        case AST_FUNC_CALL:
                ast_delete_func_call(&(node->body.func_call));
                break;
        case AST_LITERAL:
                ast_delete_literal(&(node->body.literal));
                break;
        }

        if (node->next)
                ast_delete_node(node->next);

        free(node);
}

struct ast_module *ast_parse_module(struct dom_node *node)
{
        char *name;
        int name_len;

        struct ast_node *functions;
        struct ast_node *current;

        struct dom_node *child;
        int children_count;

        struct ast_module *result;

        int i;

        name = NULL;
        functions = NULL;
        current = NULL;

        child = NULL;

        // Validate node.
        if (node->type != DOM_COMPOUND ||
            node->body.compound.type != DOM_CPD_CORE) {
                LOG_TRACE("DOM node not core compound.");
                goto error;
        }

        child = node->body.compound.children;
        children_count = node->body.compound.children_count;

        if (children_count < 2) {
                LOG_TRACE("Incorrect numger of node children :%d.", children_count);
                goto error;
        }

        if (child->type != DOM_ATOM ||
            !ast_is_keyword(child->body.atom.string, AST_KEYWORD_MODULE)) {
                LOG_TRACE("Moudle keyword not found, \"%s\" encountered instead.",
                        child->body.atom.string);
                goto error;
        }

        ++child;

        if (child->type != DOM_ATOM) {
                LOG_TRACE("Module name not found.");
                goto error;
        }

        // Parse node.
        name_len = strlen(child->body.atom.string);
        name = malloc(name_len + 1);
        strcpy(name, child->body.atom.string);

        ++child;

        LOG_TRACE("Parsed module name \"%s\".", name);

        for (i = 2; i < children_count; ++i, child++) {

                struct ast_node *node;
                node = ast_parse_expression(child);

                if (node != NULL && node->type == AST_FUNC_DECL) {
                        ast_push(node, &functions, &current);
                } else {
                        goto error;
                }
        }

        result = malloc(sizeof(*result));
        result->name = name;
        result->functions = functions;

        return result;

error:
        if (name)
                free(name);

        if (functions)
                ast_delete_node(functions);

        return NULL;
}

void ast_delete_module(struct ast_module *module)
{
        if (!module)
                return;

        if (module->functions)
                ast_delete_node(module->functions);

        free(module);
}
