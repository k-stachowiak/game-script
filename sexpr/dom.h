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

#ifndef DOM_H
#define DOM_H

#include <stdio.h>
#include <stdbool.h>

#include "sexpr/tok.h"

enum dom_node_type
{
        DOM_ATOM,
        DOM_COMPOUND
};

enum dom_cpd_type
{
        DOM_CPD_CORE,
        DOM_CPD_LIST,
        DOM_CPD_ARRAY,
        DOM_CPD_TUPLE
};

struct dom_node
{
        enum dom_node_type type;
        union
        {
                struct
                {
                        char *string;
                } atom;
                struct
                {
                        enum dom_cpd_type type;
                        struct dom_node *children;
                        int children_count;
                } compound;
        } body;
};

bool domize(struct tok_def *tok_defs, int tok_count, struct dom_node **root);

char *dom_print(struct dom_node *node);
void dom_delete_node(struct dom_node *node);

#endif
