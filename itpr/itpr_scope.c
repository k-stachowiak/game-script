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

#include <stdlib.h>
#include <string.h>

#include "common/logg.h"
#include "itpr/itpr.h"

static bool func_kvp_push(struct func_kvp *kvp,
                          struct func_kvp **kvps,
                          int *kvp_count,
                          int *kvp_cap)
{
        // Adjust capacity if needed.
        if (*kvp_cap == 0) {
                *kvp_count = 0;
                *kvp_cap = 10;
                *kvps = malloc(*kvp_cap * sizeof(**kvps));
                if (!(*kvps)) {
                        LOG_ERROR("Memory allovation failed.");
                        return false;
                }

        } else if (*kvp_cap == *kvp_count) {
                *kvp_cap *= 2;
                *kvps = realloc(*kvps, *kvp_cap * sizeof(**kvps));
                if (!(*kvps)) {
                        LOG_ERROR("Memory allocation failed.");
                        return false;
                }
        }

        // Perform the insertion.
        (*kvps)[*kvp_count] = *kvp;
        ++(*kvp_count);

        return true;
}

static bool val_kvp_push(struct val_kvp *kvp,
                         struct val_kvp **kvps,
                         int *kvp_count,
                         int *kvp_cap)
{
        // Adjust capacity if needed.
        if (*kvp_cap == 0) {
                *kvp_count = 0;
                *kvp_cap = 10;
                *kvps = malloc(*kvp_cap * sizeof(**kvps));
                if (!(*kvps)) {
                        LOG_ERROR("Memory allovation failed.");
                        return false;
                }

        } else if (*kvp_cap == *kvp_count) {
                *kvp_cap *= 2;
                *kvps = realloc(*kvps, *kvp_cap * sizeof(**kvps));
                if (!(*kvps)) {
                        LOG_ERROR("Memory allocation failed.");
                        return false;
                }
        }

        // Perform the insertion.
        (*kvps)[*kvp_count] = *kvp;
        ++(*kvp_count);

        return true;
}

struct scope *scope_build(struct ast_node *nodes, int nodes_count)
{
        struct scope *result;

        struct func_kvp fkvp;
        struct val_kvp vkvp;

        int func_kvps_cap;
        int val_kvps_cap;
        int i;

        result = malloc(sizeof(*result));
        result->parent = NULL;
        result->val_kvps = NULL;
        result->val_kvps_count = 0;
        result->func_kvps = NULL;
        result->func_kvps_count = 0;

        func_kvps_cap = 0;
        val_kvps_cap = 0;

        for (i = 0; i < nodes_count; ++i) {
                switch (nodes[i].type) {
                case AST_FUNC_DECL:
                        fkvp.key = nodes[i].body.func_decl.symbol;
                        fkvp.fdecl = nodes + i;
                        if (!func_kvp_push(&fkvp,
                                           &(result->func_kvps),
                                           &(result->func_kvps_count),
                                           &func_kvps_cap)) {
                                goto error;
                        }
                        break;
                case AST_BIND:
                        vkvp.key = nodes[i].body.bind.symbol;
                        vkvp.val = eval(nodes[i].body.bind.expr, result);
                        if (!val_kvp_push(&vkvp,
                                          &(result->val_kvps),
                                          &(result->val_kvps_count),
                                          &val_kvps_cap)) {
                                goto error;
                        }
                        break;
                case AST_FUNC_CALL:
                case AST_LITERAL:
                case AST_REFERENCE:
                case AST_COMPOUND:
                        break;
                }
        }

        return result;

error:
        if (result) {
                scope_delete(result);
                free(result);
        }

        return NULL;
}

void scope_delete(struct scope* scp)
{
        int i;

        if (!scp) {
                return;
        }

        for (i = 0; i < scp->val_kvps_count; ++i) {
                val_delete(scp->val_kvps[i].val);
        }

        free(scp->val_kvps);
        free(scp->func_kvps);
}

struct value *scope_find_val(struct scope *scp, char *symbol)
{
        struct val_kvp *kvps;
        int i;

        kvps = scp->val_kvps;
        for (i = 0; i < scp->val_kvps_count; ++i) {
                if (strcmp(symbol, kvps[i].key) == 0)
                        return kvps[i].val;
        }

        return NULL;
}

struct ast_node *scope_find_func(struct scope *scp, char *symbol)
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

