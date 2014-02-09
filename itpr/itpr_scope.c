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

#include "itpr/itpr.h"

static bool parse_functions(struct ast_node *nodes,
                            int nodes_count,
                            struct func_kvp **func_kvps,
                            int func_kvps_count,
                            int func_kvps_cap)
{
        int i;

        for (i = 0; i < nodes_count; ++i) {

                struct func_kvp kvp;

                if (nodes[i].type != AST_FUNC_DECL) {
                        continue;
                }

                kvp.key = nodes[i].func_decl.symbol;
                kvp.fdecl = nodes + i;
                if (!func_kvp_push(&kvp, func_kvps, func_kvps_count, func_kvps_cap)) {
                        return false;
                }
        }

        return true;
}

static bool parse_values(struct scope *partial_scp,
                         struct ast_node *nodes,
                         int nodes_count,
                         struct val_kvp **val_kvps,
                         int val_kvps_count,
                         int val_kvps_cap)
{
        int i;

        for (i = 0; i < nodes_count; ++i) {


        }

        return true;
}

struct scope *scope_build(struct ast_node *nodes, int nodes_count)
{
        struct scope *result;

        struct func_kvp *func_kvps;
        int func_kvps_count;
        int func_kvps_cap;

        struct val_kvp *val_kvps;
        int val_kvps_count;
        int val_kvps_cap;

        int i;

        // 1. Parse functions.
        // -------------------

        if (!parse_functions(nodes,
                             nodes_count,
                             &func_kvps,
                             &func_kvps_count,
                             &func_kvps_cap)) {
                LOG_ERROR("Failed parsing functions.");
                goto error;
        }

        //
        result = malloc(sizeof(*result));
        result->parent = NULL;

        if (!parse_values(func_kvps,
                          func_kvps_count,
                          nodes,
                          nodes_count,
                          &val_kvps,
                          &val_kvps_count,
                          &val_kvps_cap)) {
                LOG_ERROR("Failed parsing functions.");
                goto error;
        }

        result->val_kvps = val_kvps;
        result->val_kvps_count = val_kvps_count;
        result->func_kvps = func_kvps;
        result->func_kvps_count = func_kvps_count;

        return result;

error:
        // Note: function declaration kvp intestents aren't deleted here, because
        // they only store non-owning pointers to the related resources.

        if (i > 0) {
                free(func_kvps);
        }

        for (i = 0; i < val_kvps_count; ++i) {
                free(func_kvps[i].key);
                val_delete(&(func_kvps[i].val));
        }

        if (i > 0) {
                free(val_kvps);
        }

        return NULL;
}

void scope_delete(struct scope* scp)
{
        int i;

        for (i = 0; i < scp->val_kvps_count; ++i)
                free(scp->val_kvps[i].key);

        free(scp->val_kvps);
        free(scp->func_kvps);

        free(scp);
}

struct value *scope_find_val(struct scope *scp, char *symbol)
{
        struct val_kvp *kvps;
        int i;

        kvps = scp->val_kvps;
        for (i = 0; i < scp->val_kvps_count; ++i) {
                if (strcmp(symbol, kvps[i].key) == 0)
                        return &(kvps[i].value);
        }

        return NULL;
}

struct ast_func_decl *scope_find_func(struct scope *scp, char *symbol)
{
        struct func_kvp *kvps;
        int i;

        kvps = scp->func_kvps;
        for (i = 0; i < scp->func_kvps_count; ++i) {
                if (strcmp(symbol, kvps[i].key) == 0)
                        return kvps[i].fdecl;
        }

        return NULL;
}

