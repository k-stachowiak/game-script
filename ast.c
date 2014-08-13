/* Copyright (C) 2014 Krzysztof Stachowiak */

#include <stdlib.h>

#include "error.h"
#include "ast.h"

struct AstNode *ast_make_bind(
    struct Location loc,
    char *symbol, struct AstNode *expr)
{
    struct AstNode *result = malloc(sizeof(*result));

    if (!result) {
        err_set(ERR_PARSE, "Allocation failed.");
        return NULL;
    }

    result->next = NULL;
    result->type = AST_BIND;
    result->loc = loc;
    result->data.bind.symbol = symbol;
    result->data.bind.expr = expr;
    return result;
}

struct AstNode *ast_make_compound(
    struct Location loc,
    enum AstCompoundType type,
    struct AstNode *exprs)
{
    struct AstNode *result = malloc(sizeof(*result));

    if (!result) {
        err_set(ERR_PARSE, "Allocation failed.");
        return NULL;
    }

    result->next = NULL;
    result->type = AST_COMPOUND;
    result->loc = loc;
    result->data.compound.type = type;
    result->data.compound.exprs = exprs;
    return result;
}

struct AstNode *ast_make_func_call(
    struct Location loc,
    char *symbol, struct AstNode *args)
{
    struct AstNode *result = malloc(sizeof(*result));

    if (!result) {
        err_set(ERR_PARSE, "Allocation failed.");
        return NULL;
    }

    result->next = NULL;
    result->type = AST_FUNC_CALL;
    result->loc = loc;
    result->data.func_call.symbol = symbol;
    result->data.func_call.actual_args = args;
    return result;
}

struct AstNode *ast_make_func_def(
    struct Location loc,
    char **formal_args,
    int arg_count,
    struct AstNode *exprs)
{
    struct AstNode *result = malloc(sizeof(*result));

    if (!result) {
        err_set(ERR_PARSE, "Allocation failed.");
        return NULL;
    }

    result->next = NULL;
    result->type = AST_FUNC_DEF;
    result->loc = loc;
    result->data.func_def.func.formal_args = formal_args;
    result->data.func_def.func.arg_count = arg_count;
    result->data.func_def.exprs = exprs;
    return result;
}

struct AstNode *ast_make_literal_bool(struct Location loc, int value)
{
    struct AstNode *result = malloc(sizeof(*result));

    if (!result) {
        err_set(ERR_PARSE, "Allocation failed.");
        return NULL;
    }

    result->next = NULL;
    result->type = AST_LITERAL;
    result->loc = loc;
    result->data.literal.type = AST_LIT_BOOL;
    result->data.literal.data.boolean = value;
    return result;
}

struct AstNode *ast_make_literal_string(struct Location loc, char *value)
{
    struct AstNode *result = malloc(sizeof(*result));

    if (!result) {
        err_set(ERR_PARSE, "Allocation failed.");
        return NULL;
    }

    result->next = NULL;
    result->type = AST_LITERAL;
    result->loc = loc;
    result->data.literal.type = AST_LIT_STRING;
    result->data.literal.data.string = value;
    return result;
}

struct AstNode *ast_make_literal_character(struct Location loc, char value)
{
    struct AstNode *result = malloc(sizeof(*result));

    if (!result) {
        err_set(ERR_PARSE, "Allocation failed.");
        return NULL;
    }

    result->next = NULL;
    result->type = AST_LITERAL;
    result->loc = loc;
    result->data.literal.type = AST_LIT_CHAR;
    result->data.literal.data.character = value;
    return result;
}

struct AstNode *ast_make_literal_int(struct Location loc, long value)
{
    struct AstNode *result = malloc(sizeof(*result));

    if (!result) {
        err_set(ERR_PARSE, "Allocation failed.");
        return NULL;
    }

    result->next = NULL;
    result->type = AST_LITERAL;
    result->loc = loc;
    result->data.literal.type = AST_LIT_INT;
    result->data.literal.data.integer = value;
    return result;
}

struct AstNode *ast_make_literal_real(struct Location loc, double value)
{
    struct AstNode *result = malloc(sizeof(*result));

    if (!result) {
        err_set(ERR_PARSE, "Allocation failed.");
        return NULL;
    }

    result->next = NULL;
    result->type = AST_LITERAL;
    result->loc = loc;
    result->data.literal.type = AST_LIT_REAL;
    result->data.literal.data.real = value;
    return result;
}

struct AstNode *ast_make_reference(struct Location loc, char *symbol)
{
    struct AstNode *result = malloc(sizeof(*result));

    if (!result) {
        err_set(ERR_PARSE, "Allocation failed.");
        return NULL;
    }

    result->next = NULL;
    result->type = AST_REFERENCE;
    result->loc = loc;
    result->data.reference.symbol = symbol;
    return result;
}

static void ast_common_func_free(struct AstCommonFunc *acf)
{
    int i;
    for (i = 0; i < acf->arg_count; ++i) {
        free(acf->formal_args[i]);
    }
    free(acf->formal_args);
}

static void ast_bif_free(struct AstBif *abif)
{
    ast_common_func_free(&(abif->func));
}

static void ast_bind_free(struct AstBind *abind)
{
    free(abind->symbol);
    ast_node_free(abind->expr);
}

static void ast_compound_free(struct AstCompound *acpd)
{
    ast_node_free(acpd->exprs);
}

static void ast_func_call_free(struct AstFuncCall *afcall)
{
    free(afcall->symbol);
    ast_node_free(afcall->actual_args);
}

static void ast_func_def_free(struct AstFuncDef *afdef)
{
    ast_common_func_free(&(afdef->func));
    ast_node_free(afdef->exprs);
}

static void ast_literal_free(struct AstLiteral *alit)
{
    if (alit->type == AST_LIT_STRING) {
        free(alit->data.string);
    }
}

static void ast_reference_free(struct AstReference *aref)
{
    free(aref->symbol);
}

void ast_node_free(struct AstNode *current)
{
    struct AstNode *next;

    while (current) {
        switch (current->type) {
        case AST_BIF:
            ast_bif_free(&(current->data.bif));
            break;
        case AST_BIND:
            ast_bind_free(&(current->data.bind));
            break;
        case AST_COMPOUND:
            ast_compound_free(&(current->data.compound));
            break;
        case AST_FUNC_CALL:
            ast_func_call_free(&(current->data.func_call));
            break;
        case AST_FUNC_DEF:
            ast_func_def_free(&(current->data.func_def));
            break;
        case AST_LITERAL:
            ast_literal_free(&(current->data.literal));
            break;
        case AST_REFERENCE:
            ast_reference_free(&(current->data.reference));
            break;
        }
        next = current->next;
        free(current);
        current = next;
    }
}

int ast_list_len(struct AstNode *head)
{
	int result = 0;
	while (head) {
		head = head->next;
		++result;
	}
	return result;
}

void ast_visit(struct AstNode *node, void (*f)(struct AstNode*))
{
    f(node);
    switch (node->type) {
    case AST_BIF:
    case AST_LITERAL:
    case AST_REFERENCE:
        break;
    case AST_BIND:
        ast_visit(node->data.bind.expr, f);
        break;
    case AST_COMPOUND:
        if (node->data.compound.exprs) {
            ast_visit(node->data.compound.exprs, f);
        }
        break;
    case AST_FUNC_CALL:
        ast_visit(node->data.func_call.actual_args, f);
        break;
    case AST_FUNC_DEF:
        ast_visit(node->data.func_def.exprs, f);
        break;
    }
    if (node->next) {
        ast_visit(node->next, f);
    }
}

