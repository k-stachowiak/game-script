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

struct scope *scope_build(struct ast_node *first_func)
{
        struct scope *result;
        struct ast_node *current;
        struct func_kvp *func_kvps;
        int func_count;
        int i;

        func_count = ast_count(first_func);
        func_kvps = malloc(sizeof(*func_kvps) * func_count);

        current = first_func;
        for (i = 0; i < func_count; ++i) {
                func_kvps[i].key = current->body.func_decl.symbol;
                func_kvps[i].fdecl = &(current->body.func_decl);
                current = current->next;
        }

        result = malloc(sizeof(*result));
        result->parent = NULL;
        result->val_kvps = NULL;
        result->val_kvps_count = 0;
        result->func_kvps = func_kvps;
        result->func_kvps_count = func_count;

        return result;
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

