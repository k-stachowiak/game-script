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

// TODO: Handle the result value of this correctly.
bool ast_push(struct ast_node *node,
              struct ast_node **ast_nodes,
              int *ast_count,
              int *ast_cap)
{
        // Adjust capacity if needed.
        if (*ast_cap == 0) {
                *ast_count = 0;
                *ast_cap = 10;
                *ast_nodes = malloc(*ast_cap * sizeof(**ast_nodes));
                if (!(*ast_nodes)) {
                        LOG_ERROR("Memory allocation failed.");
                        return false;
                }

        } else if (*ast_cap == *ast_count) {
                *ast_cap *= 2;
                *ast_nodes = realloc(*ast_nodes, *ast_cap * sizeof(**ast_nodes));
                if (!(*ast_nodes)) {
                        LOG_ERROR("Memory allocation failed.");
                        return false;
                }
        }

        // Perform the insertion.
        (*ast_nodes)[*ast_count] = *node;
        ++(*ast_count);

        return true;
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
                result->type = AST_FUNC_DECL;
                result->body.func_decl = *fd;
                free(fd);
                return result;
        }

        if ((fc = ast_parse_func_call(node))) {
                result = malloc(sizeof(*result));
                result->type = AST_FUNC_CALL;
                result->body.func_call = *fc;
                free(fc);
                return result;
        }

        if ((lit = ast_parse_literal(node))) {
                result = malloc(sizeof(*result));
                result->type = AST_LITERAL;
                result->body.literal = *lit;
                free(lit);
                return result;
        }

        if ((ref = ast_parse_reference(node))) {
                result = malloc(sizeof(*result));
                result->type = AST_REFERENCE;
                result->body.reference = *ref;
                free(ref);
                return result;
        }

        if ((cpd = ast_parse_compound(node))) {
                result = malloc(sizeof(*result));
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
}

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

                struct ast_node *node;
                node = ast_parse_expression(dom_children + i);

                if (node != NULL && node->type == AST_FUNC_DECL) {
                        ast_push(node, &functions, &functions_count, &functions_cap);

                } else {
                        goto error;

                }
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

        if (!unit)
                return;

        for (i = 0; i < unit->functions_count; ++i) {
                ast_delete_node(unit->functions + i);
        }

        if (i > 0) {
                free(unit->functions);
        }
}
