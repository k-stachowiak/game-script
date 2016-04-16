/* Copyright (C) 2014,2015 Krzysztof Stachowiak */

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "log.h"
#include "memory.h"
#include "ast.h"

struct AstNode *ast_make_ctl_do(
        struct SourceLocation *loc,
        struct AstNode* exprs)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    result->next = NULL;
    result->type = AST_CONTROL;
    result->loc = *loc;

    result->data.control.type = AST_CTL_DO;
    result->data.control.data.doo.exprs = exprs;

    return result;
}

struct AstNode *ast_make_ctl_bind(
        struct SourceLocation *loc,
        struct Pattern *pattern,
        struct AstNode *expr)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    result->next = NULL;
    result->type = AST_CONTROL;
    result->loc = *loc;

    result->data.control.type = AST_CTL_BIND;
    result->data.control.data.bind.pattern = pattern;
    result->data.control.data.bind.expr = expr;

    return result;
}

struct AstNode *ast_make_ctl_match(
        struct SourceLocation *loc,
        struct AstNode *expr,
        struct Pattern *keys,
        struct AstNode *values)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    result->next = NULL;
    result->type = AST_CONTROL;
    result->loc = *loc;

    result->data.control.type = AST_CTL_MATCH;
    result->data.control.data.match.expr = expr;
    result->data.control.data.match.keys = keys;
    result->data.control.data.match.values = values;

    return result;
}

struct AstNode *ast_make_ctl_fdef(
        struct SourceLocation *loc,
        struct Pattern *formal_args,
        struct SourceLocation *arg_locs,
        int arg_count,
        struct AstNode *expr)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    result->next = NULL;
    result->type = AST_CONTROL;
    result->loc = *loc;

    result->data.control.type = AST_CTL_FUNC_DEF;
    result->data.control.data.fdef.formal_args = formal_args;
    result->data.control.data.fdef.arg_locs = arg_locs;
    result->data.control.data.fdef.arg_count = arg_count;
    result->data.control.data.fdef.expr = expr;

    return result;
}

struct AstNode *ast_make_ctl_fcall(
    struct SourceLocation *loc,
    struct AstNode *func, struct AstNode *args)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    result->next = NULL;
    result->type = AST_CONTROL;
    result->loc = *loc;

    result->data.control.type = AST_CTL_FUNC_CALL;
    result->data.control.data.fcall.func = func;
    result->data.control.data.fcall.actual_args = args;

    /* The helper link thank's to which all the espressions are in one list. */
    result->data.control.data.fcall.func->next =
        result->data.control.data.fcall.actual_args;

    return result;
}

struct AstNode *ast_make_ctl_reference(
        struct SourceLocation *loc,
        char *symbol)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    result->next = NULL;
    result->type = AST_CONTROL;
    result->loc = *loc;

    result->data.control.type = AST_CTL_REFERENCE;
    result->data.control.data.reference.symbol = symbol;

    return result;
}

struct AstNode *ast_make_special(
        struct SourceLocation *loc,
        enum AstSpecialType type,
        struct AstNode *args)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    result->next = NULL;
    result->type = AST_SPECIAL;
    result->loc = *loc;

    result->data.special.type = type;
    result->data.special.args = args;

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

struct AstNode *ast_make_literal_unit(struct SourceLocation *loc)
{
    struct AstNode *result = mem_malloc(sizeof(*result));
    result->next = NULL;
    result->type = AST_LITERAL;
    result->loc = *loc;
    result->data.literal.type = AST_LIT_UNIT;
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

    int length = strlen(value);
    char *copy = mem_malloc(length + 1);
    memcpy(copy, value, length + 1);

    result->next = NULL;
    result->type = AST_LITERAL;
    result->loc = *loc;

    result->data.literal.type = AST_LIT_STRING;
    result->data.literal.data.string = copy;

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

static void ast_ctl_do_free(struct AstCtlDo *doo)
{
    ast_node_free(doo->exprs);
}

static void ast_ctl_bind_free(struct AstCtlBind *bind)
{
    pattern_free(bind->pattern);
    ast_node_free_one(bind->expr);
}

static void ast_ctl_match_free(struct AstCtlMatch *match)
{
    ast_node_free(match->expr);
    pattern_free(match->keys);
    ast_node_free(match->values);
}

static void ast_ctl_func_def_free(struct AstCtlFuncDef *fdef)
{
    pattern_free(fdef->formal_args);
    mem_free(fdef->arg_locs);
    ast_node_free(fdef->expr);
}

static void ast_ctl_func_call_free(struct AstCtlFuncCall *fcall)
{
    /*
     * Note: The func node is linked forward with the function arguments,
     * therefore they will be released here as well.
     */
    ast_node_free(fcall->func);
}

static void ast_ctl_reference_free(struct AstCtlReference *ref)
{
    mem_free(ref->symbol);
}

static void ast_control_free(struct AstControl *ctl)
{
    switch (ctl->type)
    {
    case AST_CTL_DO:
        ast_ctl_do_free(&ctl->data.doo);
        break;

    case AST_CTL_BIND:
        ast_ctl_bind_free(&ctl->data.bind);
        break;

    case AST_CTL_MATCH:
        ast_ctl_match_free(&ctl->data.match);
        break;

    case AST_CTL_FUNC_DEF:
        ast_ctl_func_def_free(&ctl->data.fdef);
        break;

    case AST_CTL_FUNC_CALL:
        ast_ctl_func_call_free(&ctl->data.fcall);
        break;

    case AST_CTL_REFERENCE:
        ast_ctl_reference_free(&ctl->data.reference);
        break;
    }
}

static void ast_special_free(struct AstSpecial *special)
{
    ast_node_free(special->args);
}

static void ast_compound_free(struct AstCompound *cpd)
{
    ast_node_free(cpd->exprs);
}

static void ast_literal_free(struct AstLiteral *literal)
{
    if (literal->type == AST_LIT_STRING) {
        mem_free(literal->data.string);
    }
}

void ast_node_free_one(struct AstNode *node)
{
    switch (node->type) {
    case AST_CONTROL:
        ast_control_free(&node->data.control);
        break;

    case AST_SPECIAL:
        ast_special_free(&node->data.special);
        break;

    case AST_COMPOUND:
        ast_compound_free(&node->data.compound);
        break;

    case AST_LITERAL:
        ast_literal_free(&node->data.literal);
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
