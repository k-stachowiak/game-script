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

enum ast_keyword {
        AST_KEYWORD_MODULE,
        AST_KEYWORD_FUNC,
        AST_KEYWORD_TRUE,
        AST_KEYWORD_FALSE,
        AST_KEYWORD_COUNT,
        AST_KEYWORD_BIND,
};

enum ast_node_type {
        AST_FUNC_DECL,
        AST_FUNC_CALL,
        AST_BIND,
        AST_LITERAL,
        AST_REFERENCE,
        AST_COMPOUND
};

enum ast_node_lit_type {
        AST_LIT_INTEGER,
        AST_LIT_REAL,
        AST_LIT_CHARACTER,
        AST_LIT_STRING,
        AST_LIT_BOOLEAN
};

enum ast_node_cpd_type {
        AST_CPD_LIST,
        AST_CPD_ARRAY,
        AST_CPD_TUPLE
};

struct ast_node;

struct ast_func_decl {
        char *symbol;
        char **form_args;
        int form_args_count;
        struct ast_node *exprs;
        int exprs_count;
};

struct ast_func_call {
        char *symbol;
        struct ast_node *act_args;
        int act_args_count;
};

struct ast_bind {
        char *symbol;
        struct ast_node *expr;
};

struct ast_literal {
        enum ast_node_lit_type type;
        union {
                long integer;
                double real;
                char character;
                char *string;
                int boolean;
        } body;
};

struct ast_reference {
        char *symbol;
};

struct ast_compound {
        enum ast_node_cpd_type type;
        struct ast_node *children;
        int children_count;
};

struct ast_node {
        enum ast_node_type type;
        union {
                struct ast_func_decl func_decl;
                struct ast_func_call func_call;
                struct ast_bind bind;
                struct ast_literal literal;
                struct ast_reference reference;
                struct ast_compound compound;
        } body;
};

struct ast_unit {
        char *name;
        // TODO: handle global binds here.
        struct ast_node *functions;
        int functions_count;
};

bool ast_is_symbol(char *string);
bool ast_is_keyword(char *string, enum ast_keyword kw);
bool ast_push(struct ast_node *node, struct ast_node **nodes, int *ast_count, int *ast_cap);

struct ast_func_decl *ast_parse_func_decl(struct dom_node *node);
void ast_delete_func_decl(struct ast_func_decl *fd);

struct ast_func_call *ast_parse_func_call(struct dom_node *node);
void ast_delete_func_call(struct ast_func_call *fc);

struct ast_bind *ast_parse_bind(struct dom_node *node);
void ast_delete_bind(struct ast_bind *b);

struct ast_literal *ast_parse_literal(struct dom_node *node);
void ast_delete_literal(struct ast_literal *lit);

struct ast_reference *ast_parse_reference(struct dom_node *node);
void ast_delete_reference(struct ast_reference *ref);

struct ast_compound *ast_parse_compound(struct dom_node *node);
void ast_delete_compound(struct ast_compound *cpd);

struct ast_node *ast_parse_expression(struct dom_node *node);
void ast_delete_node(struct ast_node *node);

struct ast_unit *ast_parse_unit(struct dom_node *dom_root);
void ast_delete_unit(struct ast_unit *unit);

#endif
