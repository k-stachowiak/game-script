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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include "common/sstream.h"
#include "common/logg.h"
#include "sexpr/dom.h"

#define DOM_PRINT_BUFFER_SIZE 2048
#define INDENT 4

// Token to DOM node mapping.
// --------------------------

static enum dom_cpd_type infer_compound_type(struct tok_def *token)
{
             if (token_is_char(token, TOK_CORE_OPEN))
                return DOM_CPD_CORE;

        else if (token_is_char(token, TOK_LIST_OPEN))
                return DOM_CPD_LIST;

        else if (token_is_char(token, TOK_ARR_OPEN))
                return DOM_CPD_ARRAY;

        else if (token_is_char(token, TOK_TUP_OPEN))
                return DOM_CPD_TUPLE;

        else
                return -1;
}

// Dynamic memory management.
// --------------------------

// TODO: Handle the return value of this function.
static bool dom_push(struct dom_node *node,
                     struct dom_node **dom_nodes,
                     int *dom_count,
                     int *dom_cap)
{
        // Adjust capacity if needed.
        if (*dom_cap == 0) {
                *dom_count = 0;
                *dom_cap = 10;
                *dom_nodes = malloc(*dom_cap * sizeof(**dom_nodes));
                if (!(*dom_nodes)) {
                        LOG_ERROR("Memory allocation failed.");
                        return false;
                }

        } else if (*dom_cap == *dom_count) {
                *dom_cap *= 2;
                *dom_nodes = realloc(*dom_nodes, *dom_cap * sizeof(**dom_nodes));
                if (!(*dom_nodes)) {
                        LOG_ERROR("Memory allocation failed.");
                        return false;
                }
        }

        // Perform the insertion.
        (*dom_nodes)[*dom_count] = *node;
        ++(*dom_count);

        return true;
}

// DOM node construction.
// ----------------------

static struct dom_node *dom_read_atom(struct tok_def **current, struct tok_def **end);
static struct dom_node *dom_read_compound(struct tok_def **current, struct tok_def **end);
static struct dom_node *dom_read_any(struct tok_def **current, struct tok_def **end);

static struct dom_node *dom_read_atom(struct tok_def **current, struct tok_def **end)
{
        struct dom_node *result;
        ptrdiff_t atom_len;

        result = malloc(sizeof(*result));
        // TODO: Handle all allocation failures.
        result->type = DOM_ATOM;

        atom_len = (*current)->end - (*current)->begin;
        result->body.atom.string = malloc(atom_len + 1);
        memcpy(result->body.atom.string, (*current)->begin, atom_len);
        result->body.atom.string[atom_len] = '\0';

        ++(*current);

        return result;
}

static struct dom_node *dom_read_compound(struct tok_def **current, struct tok_def **end)
{
        struct tok_def *begin;

        struct dom_node *children;
        int children_count;
        int children_cap;

        enum dom_cpd_type infered_type;
        struct dom_node *result;
        int i;

        begin = *current;
        children = NULL;
        children_count = 0;
        children_cap = 0;
        infered_type = infer_compound_type(*current);

        ++(*current);

        while (*current != *end) {
                struct dom_node *child;

                if (token_is_closing_paren(*current) &&
                    token_paren_match(begin, *current))
                        goto success;

                child = dom_read_any(current, end);
                dom_push(child, &children, &children_count, &children_cap);
                free(child);
        }

        // Failure.
        if (children) {
                for (i = 0; i < children_count; ++i)
                        dom_delete_node(children + i);
                free(children);
        }

        return NULL;

success:
        result = malloc(sizeof(*result));
        result->type = DOM_COMPOUND;
        result->body.compound.type = infered_type;
        result->body.compound.children = children;
        result->body.compound.children_count = children_count;

        ++(*current);

        return result;
}

static struct dom_node *dom_read_any(struct tok_def **current, struct tok_def **end)
{
             if (token_is_char(*current, TOK_CORE_OPEN))
                return dom_read_compound(current, end);

        else if (token_is_char(*current, TOK_LIST_OPEN))
                return dom_read_compound(current, end);

        else if (token_is_char(*current, TOK_ARR_OPEN))
                return dom_read_compound(current, end);

        else if (token_is_char(*current, TOK_TUP_OPEN))
                return dom_read_compound(current, end);

        else
                return dom_read_atom(current, end);
}

bool dom_build(struct tok_def *tok_defs, int tok_count, struct dom_node **root)
{
        struct tok_def *begin;
        struct tok_def *end;

        begin = tok_defs;
        end = tok_defs + tok_count;
        *root = dom_read_any(&begin, &end);

        return *root && begin == end;
}

static void dom_print_impl(struct sstream *ss, struct dom_node *node, int indent)
{
        int i;
        char *cpd_type_str;

        switch (node->type) {
        case DOM_ATOM:
                for (i = 0; i < indent * INDENT; ++i) ss_printf(ss, " ");
                ss_printf(ss, "atom : \"%s\"\n", node->body.atom.string);
                break;

        case DOM_COMPOUND:
                switch (node->body.compound.type) {
                case DOM_CPD_CORE:
                        cpd_type_str = "core";
                        break;
                case DOM_CPD_LIST:
                        cpd_type_str = "list";
                        break;
                case DOM_CPD_ARRAY:
                        cpd_type_str = "array";
                        break;
                case DOM_CPD_TUPLE:
                        cpd_type_str = "tuple";
                        break;
                }

                for (i = 0; i < indent * INDENT; ++i) ss_printf(ss, " ");
                ss_printf(ss, "BEGIN compound %s\n", cpd_type_str);

                for (i = 0; i < node->body.compound.children_count; ++i)
                        dom_print_impl(ss, node->body.compound.children + i, indent + 1);

                for (i = 0; i < indent * INDENT; ++i) ss_printf(ss, " ");
                ss_printf(ss, "END compound %s\n", cpd_type_str);
                break;
        }
}

char *dom_print(struct dom_node *node)
{
        struct sstream *ss = ss_init(DOM_PRINT_BUFFER_SIZE);

        dom_print_impl(ss, node, 0);

        char *result = malloc(ss->off + 1);
        strcpy(result, ss->buffer);

        ss_delete(ss);

        return result;
}

void dom_delete_node(struct dom_node *node)
{
        int i;

        switch (node->type) {
        case DOM_ATOM:
                free(node->body.atom.string);
                break;
        default:
                // TODO: Get all the deleters right! WHO CALLS FREE???
                for (i = 0; i < node->body.compound.children_count; ++i)
                        dom_delete_node(node->body.compound.children + i);
                break;
        }
}

