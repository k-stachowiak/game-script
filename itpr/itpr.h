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

#ifndef EVAL_H
#define EVAL_H

#include "ast/ast.h"

enum val_atom_type {
        VAL_ATOM_INTEGER,
        VAL_ATOM_REAL,
        VAL_ATOM_CHARACTER,
        VAL_ATOM_STRING,
        VAL_ATOM_BOOLEAN
};

enum value_type {
        VAL_ATOMIC,
        VAL_LIST,
        VAL_ARRAY,
        VAL_TUPLE
};

struct val_atom {
        enum val_atom_type type;
        union {
                long integer;
                double real;
                char character;
                char *string;
                int boolean;
        } body;
};

struct value {
        enum value_type type;
        union {
                struct val_atom atom;
                struct {
                        struct value *values;
                        int values_count;
                } compound;
        } body;
};

struct val_kvp {
        char *key;              // non-owning.
        struct value *val;      // owning!
};

struct func_kvp {
        char *key;              // non-owning.
        struct ast_node *fdecl; // non-owning.
};

struct scope {
        struct scope *parent;

        struct val_kvp *val_kvps;
        int val_kvps_count;

        struct func_kvp *func_kvps;
        int func_kvps_count;
};

void val_copy(struct value *dst, struct value *src);
void val_delete(struct value *val);

struct scope *scope_build(struct ast_node *nodes, int nodes_count);
void scope_delete(struct scope* scp);

struct value *scope_find_val(struct scope *scp, char *symbol);
struct ast_node *scope_find_func(struct scope *scp, char *symbol);

struct value *eval(struct ast_node *expr, struct scope *scp);

#endif
