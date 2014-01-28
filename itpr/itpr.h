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

#define ITPR_MAX_KEY_LENGTH 1024

#include "ast/ast.h"

enum val_atom_type
{
        VAL_ATOM_INTEGER,
        VAL_ATOM_REAL,
        VAL_ATOM_CHARACTER,
        VAL_ATOM_STRING,
        VAL_ATOM_BOOLEAN
};

enum value_type
{
        VAL_ATOMIC,
        VAL_LIST,
        VAL_ARRAY,
        VAL_TUPLE
};

struct val_atom
{
        enum val_atom_type type;
        union
        {
                long integer;
                double real;
                char character;
                char *string;
                int boolean;
        } body;
};

struct value
{
        enum value_type type;
        union
        {
                struct val_atom atom;
                struct
                {
                        struct val_atom *atoms;
                        int atoms_count;
                } compound;
        } body;
};

struct kvp
{
        char key[ITPR_MAX_KEY_LENGTH];
        value val;
};

struct scope
{
        struct scope *parent;
        struct kvp *kvps;
        int kvps_count;
};

struct value *eval(struct ast_node *expr, struct scope *scp);

#endif
