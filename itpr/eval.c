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

#include "common/logg.h"
#include "itpr/itpr.h"

static struct value *eval_func_decl(struct ast_func_decl*, struct scope*)
{
        LOG_ERROR("Higher order functions not supported.");
        return NULL;
}

static struct value *eval_func_call(struct ast_func_call *fc, struct scope *scp)
{
}

static struct value *eval_literal(struct ast_literal *lit)
{
}

static struct value *eval_reference(struct ast_reference *ref, struct scope *scp)
{
}

static struct value *eval_list(struct ast_node *first_expr, struct scope *scp)
{
}

static struct value *eval_array(struct ast_node *first_expr, struct scope *scp)
{
}

static struct value *eval_tuple(struct ast_node *first_expr, struct scope *scp)
{
}

struct value *eval(struct ast_node *expr, struct scope *scp)
{
        switch (expr->type) {
        case AST_FUNC_DECL:
                return eval_func_decl(&(expr->body.func_decl), scp);

        case AST_FUNC_CALL:
                return eval_func_call(&(expr->body.func_call), scp);

        case AST_LITERAL:
                return eval_literal(&(expr->body.literal));

        case AST_REFERENCE:
                return eval_reference(&(expr->body.reference), scp);

        case AST_COMPOUND:
                switch (expr->body.compound.type) {
                case AST_CPD_LIST:
                        return eval_list(expr->body.compound.first_element, scp);

                case AST_CPD_ARRAY:
                        return eval_array(expr->body.compound.first_element, scp);

                case AST_CPD_TUPLE:
                        return eval_tuple(expr->body.compound.first_element, scp);
                }
        }
}
