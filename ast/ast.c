/* Copyright (C) 2014,2015 Krzysztof Stachowiak */

#include <stdlib.h>
#include <assert.h>

#include "memory.h"
#include "error.h"
#include "ast.h"

struct AstNode *ast_make_do_block(
        struct SourceLocation *loc,
        struct AstNode* exprs)
{
    struct AstNode *result = mem_malloc(sizeof(*result));
    result->next = NULL;
    result->type = AST_DO_BLOCK;
    result->loc = *loc;
    result->data.do_block.exprs = exprs;
    return result;
}

struct AstNode *ast_make_bind(
        struct SourceLocation *loc,
        struct Pattern *pattern,
        struct AstNode *expr)
{
    struct AstNode *result = mem_malloc(sizeof(*result));
    result->next = NULL;
    result->type = AST_BIND;
    result->loc = *loc;
    result->data.bind.pattern = pattern;
    result->data.bind.expr = expr;
    return result;
}

struct AstNode *ast_make_compound(
    struct SourceLocation *loc,
    enum AstCompoundType type,
    struct AstNode *exprs)
{
    struct AstNode *result = mem_malloc(sizeof(*result));
    result->next = NULL;
    result->type = AST_COMPOUND;
    result->loc = *loc;
    result->data.compound.type = type;
    result->data.compound.exprs = exprs;
    return result;
}

struct AstNode *ast_make_func_call(
    struct SourceLocation *loc,
    char *symbol, struct AstNode *args)
{
    struct AstNode *result = mem_malloc(sizeof(*result));
    result->next = NULL;
    result->type = AST_FUNC_CALL;
    result->loc = *loc;
    result->data.func_call.symbol = symbol;
    result->data.func_call.actual_args = args;
    return result;
}

struct AstNode *ast_make_func_def(
        struct SourceLocation *loc,
        struct Pattern *formal_args,
        struct SourceLocation *arg_locs,
        int arg_count,
        struct AstNode *expr)
{
    struct AstNode *result = mem_malloc(sizeof(*result));
    result->next = NULL;
    result->type = AST_FUNC_DEF;
    result->loc = *loc;
    result->data.func_def.formal_args = formal_args;
    result->data.func_def.arg_locs = arg_locs;
    result->data.func_def.arg_count = arg_count;
    result->data.func_def.expr = expr;
    return result;
}

struct AstNode *ast_make_parafunc(
    struct SourceLocation *loc,
    enum AstParafuncType type,
    struct AstNode *args)
{
    struct AstNode *result = mem_malloc(sizeof(*result));
    result->next = NULL;
    result->type = AST_PARAFUNC;
    result->loc = *loc;
    result->data.parafunc.type = type;
    result->data.parafunc.args = args;
    return result;
}

struct AstNode *ast_make_literal_bool(struct SourceLocation *loc, int value)
{
    struct AstNode *result = mem_malloc(sizeof(*result));
    result->next = NULL;
    result->type = AST_LITERAL;
    result->loc = *loc;
    result->data.literal.type = AST_LIT_BOOL;
    result->data.literal.data.boolean = value;
    return result;
}

struct AstNode *ast_make_literal_string(struct SourceLocation *loc, char *value)
{
    struct AstNode *result = mem_malloc(sizeof(*result));
    result->next = NULL;
    result->type = AST_LITERAL;
    result->loc = *loc;
    result->data.literal.type = AST_LIT_STRING;
    result->data.literal.data.string = value;
    return result;
}

struct AstNode *ast_make_literal_character(struct SourceLocation *loc, char value)
{
    struct AstNode *result = mem_malloc(sizeof(*result));
    result->next = NULL;
    result->type = AST_LITERAL;
    result->loc = *loc;
    result->data.literal.type = AST_LIT_CHAR;
    result->data.literal.data.character = value;
    return result;
}

struct AstNode *ast_make_literal_int(struct SourceLocation *loc, long value)
{
    struct AstNode *result = mem_malloc(sizeof(*result));
    result->next = NULL;
    result->type = AST_LITERAL;
    result->loc = *loc;
    result->data.literal.type = AST_LIT_INT;
    result->data.literal.data.integer = value;
    return result;
}

struct AstNode *ast_make_literal_real(struct SourceLocation *loc, double value)
{
    struct AstNode *result = mem_malloc(sizeof(*result));
    result->next = NULL;
    result->type = AST_LITERAL;
    result->loc = *loc;
    result->data.literal.type = AST_LIT_REAL;
    result->data.literal.data.real = value;
    return result;
}

struct AstNode *ast_make_reference(struct SourceLocation *loc, char *symbol)
{
    struct AstNode *result = mem_malloc(sizeof(*result));
    result->next = NULL;
    result->type = AST_REFERENCE;
    result->loc = *loc;
    result->data.reference.symbol = symbol;
    return result;
}

static void ast_do_block_free(struct AstDoBlock *adb)
{
    ast_node_free(adb->exprs);
}

static void ast_bind_free(struct AstBind *abind)
{
    pattern_free(abind->pattern);
    ast_node_free_one(abind->expr);
}

static void ast_compound_free(struct AstCompound *acpd)
{
    ast_node_free(acpd->exprs);
}

static void ast_func_call_free(struct AstFuncCall *afcall)
{
    mem_free(afcall->symbol);
    ast_node_free(afcall->actual_args);
}

static void ast_func_def_free(struct AstFuncDef *afdef)
{
    pattern_free(afdef->formal_args);
    mem_free(afdef->arg_locs);
    ast_node_free(afdef->expr);
}

static void ast_literal_free(struct AstLiteral *alit)
{
    if (alit->type == AST_LIT_STRING) {
        mem_free(alit->data.string);
    }
}

static void ast_parafunc_free(struct AstParafunc *parafunc)
{
    ast_node_free(parafunc->args);
}

static void ast_reference_free(struct AstReference *aref)
{
    mem_free(aref->symbol);
}

void ast_node_free_one(struct AstNode *node)
{
    switch (node->type) {
    case AST_DO_BLOCK:
        ast_do_block_free(&(node->data.do_block));
        break;

    case AST_BIND:
        ast_bind_free(&(node->data.bind));
        break;

    case AST_COMPOUND:
        ast_compound_free(&(node->data.compound));
        break;

    case AST_FUNC_CALL:
        ast_func_call_free(&(node->data.func_call));
        break;

    case AST_FUNC_DEF:
        ast_func_def_free(&(node->data.func_def));
        break;

    case AST_LITERAL:
        ast_literal_free(&(node->data.literal));
        break;

    case AST_PARAFUNC:
        ast_parafunc_free(&(node->data.parafunc));
        break;

    case AST_REFERENCE:
        ast_reference_free(&(node->data.reference));
        break;
    }

    mem_free(node);
}

void ast_node_free(struct AstNode *current)
{
    while (current) {
        struct AstNode *next = current->next;
        ast_node_free_one(current);
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

