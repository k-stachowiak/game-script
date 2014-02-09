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

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "common/logg.h"
#include "itpr/itpr.h"

static bool value_types_equal(struct value *lhs, struct value *rhs)
{
        enum value_type common_val_type;
        enum val_atom_type common_atom_type;
        struct val_atom *latom, *ratom;
        struct value *lval, *rval;
        int common_val_count;
        int i;

        if (lhs->type != rhs->type)
                return false;

        common_val_type = lhs->type;

        switch (common_val_type) {
        case VAL_ATOMIC:
                if (lhs->body.atom.type != rhs->body.atom.type)
                        return false;

                common_atom_type = lhs->body.atom.type;
                latom = &(lhs->body.atom);
                ratom = &(rhs->body.atom);

                switch (common_atom_type) {
                case VAL_ATOM_INTEGER:
                        return latom->body.integer == ratom->body.integer;
                case VAL_ATOM_REAL:
                        return latom->body.real == ratom->body.real;
                case VAL_ATOM_CHARACTER:
                        return latom->body.character == ratom->body.character;
                case VAL_ATOM_STRING:
                        return strcmp(latom->body.string, ratom->body.string) == 0;
                case VAL_ATOM_BOOLEAN:
                        return latom->body.boolean == ratom->body.boolean;
                }

                LOG_ERROR("Logic error : control should not reach this point.");
                return false;

        case VAL_LIST:
        case VAL_ARRAY:
        case VAL_TUPLE:
                if (lhs->body.compound.values_count != rhs->body.compound.values_count)
                        return false;

                common_val_count = lhs->body.compound.values_count;
                lval = lhs->body.compound.values;
                rval = rhs->body.compound.values;
                for (i = 0; i < common_val_count; ++i) {
                        if (!value_types_equal(lval, rval))
                                return false;
                        ++lval;
                        ++rval;
                }
                return true;
        }
}

static bool node_types_equal(struct ast_node *lhs, struct ast_node *rhs, struct scope *scp)
{
        // TODO: Handle error in finding references to non registered symbols.

        // TODO: Decide how to manage type deduction in the runtime context.

        // Common.
        enum ast_node_type common_type;
        int i;

        // Function call comparison.
        struct ast_node *lfdecl, *rfdecl;
        struct ast_node *last_lexpr, *last_rexpr;

        // Bind comparison.
        struct ast_node *lbexpr, *rbexpr;

        // Literal comparison.
        enum ast_node_lit_type llit_type, rlit_type;

        // Reference comparison.
        struct value *lval, *rval;

        // Compound comparison.
        struct ast_node *lsubnode, *rsubnode;
        int lcpd_size, rcpd_size;
        int common_cpd_size;

        // Implementation.

        if (lhs->type != rhs->type)
                return false;

        common_type = lhs->type;

        switch (common_type) {
        case AST_FUNC_DECL:
                LOG_ERROR("Type comparicon not suported!");
                return false;

        case AST_FUNC_CALL:
                lfdecl = scope_find_func(scp, lhs->body.func_call.symbol);
                rfdecl = scope_find_func(scp, rhs->body.func_call.symbol);
                if (!lfdecl || !rfdecl)
                        return false;
                last_lexpr = lfdecl->body.func_decl.exprs + (lfdecl->body.func_decl.exprs_count - 1);
                last_rexpr = rfdecl->body.func_decl.exprs + (rfdecl->body.func_decl.exprs_count - 1);
                return node_types_equal(last_lexpr, last_rexpr, scp);

        case AST_BIND:
                lbexpr = lhs->body.bind.expr;
                rbexpr = rhs->body.bind.expr;
                return node_types_equal(lbexpr, rbexpr, scp);

        case AST_LITERAL:
                llit_type = lhs->body.literal.type;
                rlit_type = rhs->body.literal.type;
                return llit_type == rlit_type;

        case AST_REFERENCE:
                lval = scope_find_val(scp, lhs->body.reference.symbol);
                rval = scope_find_val(scp, rhs->body.reference.symbol);
                if (!lval && !rval)
                        return false;
                if (!lval || !rval)
                        return false;
                return value_types_equal(rval, lval);

        case AST_COMPOUND:
                if (lhs->body.compound.type != rhs->body.compound.type)
                        return false;

                lsubnode = lhs->body.compound.children;
                rsubnode = rhs->body.compound.children;
                lcpd_size = lhs->body.compound.children_count;
                rcpd_size = rhs->body.compound.children_count;
                if (lcpd_size != rcpd_size)
                        return false;

                common_cpd_size = lcpd_size;
                for (i = 0; i < common_cpd_size; ++i) {
                        if (!node_types_equal(lsubnode, rsubnode, scp)) {
                                return false;
                        }
                        ++lsubnode;
                        ++rsubnode;
                }
                return true;
        }
}

static bool all_children_eq_type(struct ast_compound *cpd, struct scope *scp)
{
        int i;

        if (cpd->children_count < 2)
                return true;

        for (i = 1; i < cpd->children_count; ++i) {
                if (!node_types_equal(cpd->children,
                                      cpd->children + i,
                                      scp)) {
                        return false;
                }
        }

        return true;
}

static struct value *eval_func_decl(struct ast_func_decl* fd, struct scope* scp)
{
        LOG_ERROR("Higher order functions not evaluable.");
        return NULL;
}

static struct value *eval_func_call(struct ast_func_call *fc, struct scope *scp)
{
        // TODO: implement.
        LOG_ERROR("Not implemented!");
        return NULL;
}

static struct value *eval_bind(struct ast_bind *b, struct scope *scp)
{
        LOG_ERROR("Binds not evaluable.");
        return NULL;
}

static struct value *eval_literal(struct ast_literal *lit)
{
        char *string;
        struct value *result;
        int string_len;

        result = malloc(sizeof(*result));

        result->type = VAL_ATOMIC;

        switch (lit->type) {
        case AST_LIT_INTEGER:
                result->body.atom.body.integer = lit->body.integer;
                break;
        case AST_LIT_REAL:
                result->body.atom.body.real = lit->body.real;
                break;
        case AST_LIT_CHARACTER:
                result->body.atom.body.character = lit->body.character;
                break;
        case AST_LIT_STRING:
                string_len = strlen(lit->body.string);
                string = malloc(string_len + 1);
                if (!string) {
                        LOG_ERROR("Memory allocation failed.");
                        return NULL;
                }
                strcpy(string, lit->body.string);
                result->body.atom.body.string = string;
                break;
        case AST_LIT_BOOLEAN:
                result->body.atom.body.boolean = lit->body.boolean;
                break;
        }

        return result;
}

static struct value *eval_reference(struct ast_reference *ref, struct scope *scp)
{
        struct value *found;
        struct value *result;

        if (!(found = scope_find_val(scp, ref->symbol)))
                return NULL;

        result = malloc(sizeof(*result));
        if (!result) {
                LOG_ERROR("Memory allocation failed.");
                return NULL;
        }

        val_copy(result, found);

        return result;
}

static struct value *eval_compound(struct ast_node *exprs,
                                   int exprs_count,
                                   struct scope *scp,
                                   enum value_type type)
{
        int i;
        struct value *result;

        assert(type != VAL_ATOMIC);

        result = malloc(sizeof(*result));
        if (!result) {
                LOG_ERROR("Memory allocation failed.");
                return NULL;
        }

        result->type = type;

        result->body.compound.values_count = exprs_count;
        result->body.compound.values = malloc(sizeof(struct value) * exprs_count);
        if (!result->body.compound.values) {
                free(result);
                LOG_ERROR("Memory allocation failed.");
                return NULL;
        }

        for (i = 0; i < exprs_count; ++i) {
                struct value *val;
                val = eval(exprs + i, scp);
                val_copy(result + i, val);
                val_delete(val);
        }

        return result;
}

void val_copy(struct value *dst, struct value *src)
{
        int values_count;
        int i;

        dst->type = src->type;

        switch (src->type) {
        case VAL_ATOMIC:
                memcpy(dst, src, sizeof(*src));
                break;
        case VAL_LIST:
        case VAL_ARRAY:
        case VAL_TUPLE:
                values_count = src->body.compound.values_count;
                dst->body.compound.values_count = values_count;
                dst->body.compound.values = malloc(values_count * sizeof(struct value));
                for (i = 0; i < values_count; ++i) {
                        val_copy(dst->body.compound.values + i,
                                 src->body.compound.values + i);
                }
                break;
        }
}

void val_delete(struct value *val)
{
        int i;

        if (!val) {
                return;
        }

        switch (val->type) {
        case VAL_ATOMIC:
                if (val->body.atom.type == VAL_ATOM_STRING) {
                        free(val->body.atom.body.string);
                }
                break;
        case VAL_LIST:
        case VAL_ARRAY:
        case VAL_TUPLE:
                for (i = 0; i < val->body.compound.values_count; ++i) {
                        val_delete(val->body.compound.values + i);
                }
                free(val->body.compound.values);
                break;
        }

        free(val);
}

struct value *eval(struct ast_node *expr, struct scope *scp)
{
        switch (expr->type) {
        case AST_FUNC_DECL:
                return eval_func_decl(&(expr->body.func_decl), scp);

        case AST_FUNC_CALL:
                return eval_func_call(&(expr->body.func_call), scp);

        case AST_BIND:
                return eval_bind(&(expr->body.bind), scp);

        case AST_LITERAL:
                return eval_literal(&(expr->body.literal));

        case AST_REFERENCE:
                return eval_reference(&(expr->body.reference), scp);

        case AST_COMPOUND:
                switch (expr->body.compound.type) {
                case AST_CPD_LIST:
                        if (!all_children_eq_type(&(expr->body.compound), scp)) {
                                LOG_ERROR("Inconsistent list elements' types.");
                                return NULL;
                        }
                        return eval_compound(expr->body.compound.children,
                                             expr->body.compound.children_count,
                                             scp,
                                             VAL_LIST);

                case AST_CPD_ARRAY:
                        if (!all_children_eq_type(&(expr->body.compound), scp)) {
                                LOG_ERROR("Inconsistent array elements' types.");
                                return NULL;
                        }
                        return eval_compound(expr->body.compound.children,
                                             expr->body.compound.children_count,
                                             scp,
                                             VAL_ARRAY);

                case AST_CPD_TUPLE:
                        return eval_compound(expr->body.compound.children,
                                             expr->body.compound.children_count,
                                             scp,
                                             VAL_TUPLE);
                }
        }
}
