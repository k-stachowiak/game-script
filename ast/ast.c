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
#include <ctype.h>

#include "common/logg.h"
#include "ast/ast.h"

bool ast_is_symbol(char *string)
{
        int i;

        if (isdigit(string[0]))
                return false;

        for (i = 0; i < AST_KEYWORD_COUNT; ++i)
                if (ast_is_keyword(string, i))
                        return false;

        return true;
}

bool ast_is_keyword(char *string, enum ast_keyword kw)
{
        static char *kw_map[] = {
                "unit",
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

int ast_count(struct ast_node *first)
{
        int result;

        result = 1;

        while ((first = first->next))
                ++result;

        return result;
}

struct ast_node *ast_fc_get_last_expr(struct ast_func_decl *fdecl)
{
        struct ast_node *result;

        result = fdecl->first_expr;
        while (result->next)
                result = result->next;

        return result;
}

struct ast_node *ast_parse_expression(struct dom_node *node)
{
        /*
         * NOTE: in this function shallow copy of the created expression is made
         * to the result structure. Therefore the objects obtained from the
         * parsing functions aren't deeply deallocated. Only the "shells" are
         * freed, whereas the "intestents" keep on living within the result
         * object.
         */

        struct ast_node *result;

        struct ast_func_decl *fd;
        struct ast_func_call *fc;
        struct ast_literal *lit;
        struct ast_reference *ref;
        struct ast_compound *cpd;

        if ((fd = ast_parse_func_decl(node))) {
                result = malloc(sizeof(*result));
                result->next = NULL;
                result->type = AST_FUNC_DECL;
                result->body.func_decl = *fd;
                free(fd);
                return result;
        }

        if ((fc = ast_parse_func_call(node))) {
                result = malloc(sizeof(*result));
                result->next = NULL;
                result->type = AST_FUNC_CALL;
                result->body.func_call = *fc;
                free(fc);
                return result;
        }

        if ((lit = ast_parse_literal(node))) {
                result = malloc(sizeof(*result));
                result->next = NULL;
                result->type = AST_LITERAL;
                result->body.literal = *lit;
                free(lit);
                return result;
        }

        if ((ref = ast_parse_reference(node))) {
                result = malloc(sizeof(*result));
                result->next = NULL;
                result->type = AST_REFERENCE;
                result->body.reference = *ref;
                free(ref);
                return result;
        }

        if ((cpd = ast_parse_compound(node))) {
                result = malloc(sizeof(*result));
                result->next = NULL;
                result->type = AST_COMPOUND;
                result->body.compound = *cpd;
                free(cpd);
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
        case AST_REFERENCE:
                ast_delete_reference(&(node->body.reference));
                break;
        case AST_COMPOUND:
                ast_delete_compound(&(node->body.compound));
                break;
        }

        if (node->next)
                ast_delete_node(node->next);

        free(node);
}

struct ast_unit *ast_parse_unit(struct dom_node *node)
{
        char *name;
        int name_len;

        struct ast_node *functions;
        struct ast_node *current;

        struct dom_node *child;
        int children_count;

        struct ast_unit *result;

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
                LOG_TRACE("Unit name not found.");
                goto error;
        }

        // Parse node.
        name_len = strlen(child->body.atom.string);
        name = malloc(name_len + 1);
        strcpy(name, child->body.atom.string);

        ++child;

        LOG_TRACE("Parsed unit name \"%s\".", name);

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

void ast_delete_unit(struct ast_unit *unit)
{
        if (!unit)
                return;

        if (unit->functions)
                ast_delete_node(unit->functions);

        free(unit);
}
