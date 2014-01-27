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

#ifndef AST_H
#define AST_H

#include "sexpr/dom.h"

enum ast_keyword
{
        AST_KEYWORD_MODULE,
        AST_KEYWORD_FUNC,
        AST_KEYWORD_TRUE,
        AST_KEYWORD_FALSE,
        AST_KEYWORD_COUNT,
};

enum ast_node_type
{
        AST_FUNC_DECL,
        AST_FUNC_CALL,
        AST_LITERAL,
};

enum ast_node_lit_type
{
        AST_LIT_INTEGER,
        AST_LIT_REAL,
        AST_LIT_CHARACTER,
        AST_LIT_STRING,
        AST_LIT_BOOLEAN
        // TODO: add compound types.
};

struct ast_node;

struct ast_func_decl
{
        char *symbol;
        char **form_args;
        int form_args_count;
        struct ast_node *first_expr;
};

struct ast_func_call
{
        char *symbol;
        struct ast_node *first_act_arg;
};

struct ast_literal {
        enum ast_node_lit_type type;
        union
        {
                long integer;
                double real;
                char character;
                char *string;
                int boolean;
        } body;
};

struct ast_node
{
        enum ast_node_type type;
        union
        {
                struct ast_func_decl func_decl;
                struct ast_func_call func_call;
                struct ast_literal literal;
        } body;
        struct ast_node *next;
};

struct ast_module
{
        char *name;
        struct ast_node *functions;
};

bool ast_is_keyword(char *string, enum ast_keyword kw);
void ast_push(struct ast_node *node, struct ast_node **begin, struct ast_node **end);

struct ast_func_decl *ast_parse_func_decl(struct dom_node *node);
void ast_delete_func_decl(struct ast_func_decl *fd);

struct ast_func_call *ast_parse_func_call(struct dom_node *node);
void ast_delete_func_call(struct ast_func_call *fc);

struct ast_literal *ast_parse_literal(struct dom_node *node);
void ast_delete_literal(struct ast_literal *lit);

struct ast_node *ast_parse_expression(struct dom_node *node);
void ast_delete_node(struct ast_node *node);

struct ast_module *ast_parse_module(struct dom_node *dom_root);
void ast_delete_module(struct ast_module *module);

#endif
