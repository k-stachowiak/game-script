/* Copyright (C) 2014,2015 Krzysztof Stachowiak */

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "log.h"
#include "memory.h"
#include "ast.h"

struct AstNode *ast_make_symbol(
        struct SourceLocation *loc,
        char *symbol)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    result->next = NULL;
    result->type = AST_SYMBOL;
    result->loc = *loc;

    result->data.symbol.symbol = symbol;

    return result;
}

struct AstNode *ast_make_spec_do(
        struct SourceLocation *loc,
        struct AstNode* exprs)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    result->next = NULL;
    result->type = AST_SPECIAL;
    result->loc = *loc;

    result->data.special.type = AST_SPEC_DO;
    result->data.special.data.doo.exprs = exprs;

    return result;
}

struct AstNode *ast_make_spec_match(
        struct SourceLocation *loc,
        struct AstNode *expr,
        struct Pattern *keys,
        struct AstNode *values)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    result->next = NULL;
    result->type = AST_SPECIAL;
    result->loc = *loc;

    result->data.special.type = AST_SPEC_MATCH;
    result->data.special.data.match.expr = expr;
    result->data.special.data.match.keys = keys;
    result->data.special.data.match.values = values;

    /* Link values list to the matched expression to ease traversal. */
    expr->next = values;

    return result;
}

struct AstNode *ast_make_spec_if(
        struct SourceLocation *loc,
        struct AstNode *test,
        struct AstNode *true_expr,
        struct AstNode *false_expr)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    result->next = NULL;
    result->type = AST_SPECIAL;
    result->loc = *loc;

    result->data.special.type = AST_SPEC_IF;
    result->data.special.data.iff.test = test;
    result->data.special.data.iff.true_expr = true_expr;
    result->data.special.data.iff.false_expr = false_expr;

    /* Link to ease releasing */
    test->next = true_expr;
    true_expr->next = false_expr;

    return result;
}

struct AstNode *ast_make_spec_while(
        struct SourceLocation *loc,
        struct AstNode *test,
        struct AstNode *expr)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    result->next = NULL;
    result->type = AST_SPECIAL;
    result->loc = *loc;

    result->data.special.type = AST_SPEC_WHILE;
    result->data.special.data.whilee.test = test;
    result->data.special.data.whilee.expr = expr;

    /* Link to ease releasing */
    test->next = expr;

    return result;
}

struct AstNode *ast_make_spec_func_def(
        struct SourceLocation *loc,
        struct Pattern *formal_args,
        struct SourceLocation *arg_locs,
        int arg_count,
        struct AstNode *expr)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    result->next = NULL;
    result->type = AST_SPECIAL;
    result->loc = *loc;

    result->data.special.type = AST_SPEC_FUNC_DEF;
    result->data.special.data.func_def.formal_args = formal_args;
    result->data.special.data.func_def.arg_locs = arg_locs;
    result->data.special.data.func_def.arg_count = arg_count;
    result->data.special.data.func_def.expr = expr;

    return result;
}

struct AstNode *ast_make_spec_and(
        struct SourceLocation *loc,
        struct AstNode *exprs)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    result->next = NULL;
    result->type = AST_SPECIAL;
    result->loc = *loc;

    result->data.special.type = AST_SPEC_AND;
    result->data.special.data.andd.exprs = exprs;

    return result;
}

struct AstNode *ast_make_spec_or(
        struct SourceLocation *loc,
        struct AstNode *exprs)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    result->next = NULL;
    result->type = AST_SPECIAL;
    result->loc = *loc;

    result->data.special.type = AST_SPEC_OR;
    result->data.special.data.orr.exprs = exprs;

    return result;
}

struct AstNode *ast_make_spec_bind(
        struct SourceLocation *loc,
        struct Pattern *pattern,
        struct AstNode *expr)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    result->next = NULL;
    result->type = AST_SPECIAL;
    result->loc = *loc;

    result->data.special.type = AST_SPEC_BIND;
    result->data.special.data.bind.pattern = pattern;
    result->data.special.data.bind.expr = expr;

    return result;
}

struct AstNode *ast_make_spec_ref(
        struct SourceLocation *loc,
        struct AstNode *expr)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    result->next = NULL;
    result->type = AST_SPECIAL;
    result->loc = *loc;

    result->data.special.type = AST_SPEC_REF;
    result->data.special.data.ref.expr = expr;

    return result;
}

struct AstNode *ast_make_spec_peek(
        struct SourceLocation *loc,
        struct AstNode *expr)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    result->next = NULL;
    result->type = AST_SPECIAL;
    result->loc = *loc;

    result->data.special.type = AST_SPEC_PEEK;
    result->data.special.data.peek.expr = expr;

    return result;
}

struct AstNode *ast_make_spec_poke(
        struct SourceLocation *loc,
        struct AstNode *reference,
        struct AstNode *value)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    result->next = NULL;
    result->type = AST_SPECIAL;
    result->loc = *loc;

    result->data.special.type = AST_SPEC_POKE;
    result->data.special.data.poke.reference = reference;
    result->data.special.data.poke.value = value;

    /* Link to ease releasing */
    reference->next = value;

    return result;
}

struct AstNode *ast_make_spec_begin(
        struct SourceLocation *loc,
        struct AstNode *collection)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    result->next = NULL;
    result->type = AST_SPECIAL;
    result->loc = *loc;

    result->data.special.type = AST_SPEC_BEGIN;
    result->data.special.data.begin.collection = collection;

    return result;
}

struct AstNode *ast_make_spec_end(
        struct SourceLocation *loc,
        struct AstNode *collection)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    result->next = NULL;
    result->type = AST_SPECIAL;
    result->loc = *loc;

    result->data.special.type = AST_SPEC_END;
    result->data.special.data.end.collection = collection;

    return result;
}

struct AstNode *ast_make_spec_inc(
        struct SourceLocation *loc,
        struct AstNode *reference)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    result->next = NULL;
    result->type = AST_SPECIAL;
    result->loc = *loc;

    result->data.special.type = AST_SPEC_INC;
    result->data.special.data.inc.reference = reference;

    return result;
}

struct AstNode *ast_make_spec_succ(
        struct SourceLocation *loc,
        struct AstNode *reference)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    result->next = NULL;
    result->type = AST_SPECIAL;
    result->loc = *loc;

    result->data.special.type = AST_SPEC_SUCC;
    result->data.special.data.succ.reference = reference;

    return result;
}

struct AstNode *ast_make_func_call(
    struct SourceLocation *loc,
    struct AstNode *func,
    struct AstNode *args)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    result->next = NULL;
    result->type = AST_FUNCTION_CALL;
    result->loc = *loc;

    result->data.func_call.func = func;
    result->data.func_call.actual_args = args;

    /* Link to ease releasing */
    func->next = args;

    return result;
}

struct AstNode *ast_make_literal_compound(
        struct SourceLocation *loc,
        enum AstLiteralCompoundType type,
        struct AstNode *exprs)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    result->next = NULL;
    result->type = AST_LITERAL_COMPOUND;
    result->loc = *loc;

    result->data.literal_compound.type = type;
    result->data.literal_compound.exprs = exprs;

    return result;
}

struct AstNode *ast_make_literal_atomic_unit(struct SourceLocation *loc)
{
    struct AstNode *result = mem_malloc(sizeof(*result));
    result->next = NULL;
    result->type = AST_LITERAL_ATOMIC;
    result->loc = *loc;
    result->data.literal_atomic.type = AST_LIT_ATOM_UNIT;
    return result;
}

struct AstNode *ast_make_literal_atomic_bool(struct SourceLocation *loc, int value)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    result->next = NULL;
    result->type = AST_LITERAL_ATOMIC;
    result->loc = *loc;

    result->data.literal_atomic.type = AST_LIT_ATOM_BOOL;
    result->data.literal_atomic.data.boolean = value;

    return result;
}

struct AstNode *ast_make_literal_atomic_string(struct SourceLocation *loc, char *value)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    int length = strlen(value);
    char *copy = mem_malloc(length + 1);
    memcpy(copy, value, length + 1);

    result->next = NULL;
    result->type = AST_LITERAL_ATOMIC;
    result->loc = *loc;

    result->data.literal_atomic.type = AST_LIT_ATOM_STRING;
    result->data.literal_atomic.data.string = copy;

    return result;
}

struct AstNode *ast_make_literal_atomic_character(struct SourceLocation *loc, char value)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    result->next = NULL;
    result->type = AST_LITERAL_ATOMIC;
    result->loc = *loc;

    result->data.literal_atomic.type = AST_LIT_ATOM_CHAR;
    result->data.literal_atomic.data.character = value;

    return result;
}

struct AstNode *ast_make_literal_atomic_int(struct SourceLocation *loc, long value)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    result->next = NULL;
    result->type = AST_LITERAL_ATOMIC;
    result->loc = *loc;

    result->data.literal_atomic.type = AST_LIT_ATOM_INT;
    result->data.literal_atomic.data.integer = value;

    return result;
}

struct AstNode *ast_make_literal_atomic_real(struct SourceLocation *loc, double value)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    result->next = NULL;
    result->type = AST_LITERAL_ATOMIC;
    result->loc = *loc;

    result->data.literal_atomic.type = AST_LIT_ATOM_REAL;
    result->data.literal_atomic.data.real = value;

    return result;
}

static void ast_symbol_free(struct AstSymbol *symbol)
{
    mem_free(symbol->symbol);
}

static void ast_special_do_free(struct AstSpecDo *doo)
{
    ast_node_free(doo->exprs);
}

static void ast_special_match_free(struct AstSpecMatch *match)
{
    ast_node_free(match->expr);
    pattern_free(match->keys);
    /* The expression node is linked with the values list. */
}

static void ast_special_if_free(struct AstSpecIf *iff)
{
    ast_node_free(iff->test);
    /* All the nodes in the iff object are linked together. */
}

static void ast_special_while_free(struct AstSpecWhile *whilee)
{
    ast_node_free(whilee->test);
    /* All the nodes in the whilee object are linked together. */
}

static void ast_special_func_def_free(struct AstSpecFuncDef *func_def)
{
    pattern_free(func_def->formal_args);
    mem_free(func_def->arg_locs);
    ast_node_free(func_def->expr);
}

static void ast_special_and_free(struct AstSpecAnd *andd)
{
    ast_node_free(andd->exprs);
}

static void ast_special_or_free(struct AstSpecOr *orr)
{
    ast_node_free(orr->exprs);
}

static void ast_special_bind_free(struct AstSpecBind *bind)
{
    pattern_free(bind->pattern);
    ast_node_free_one(bind->expr);
}

static void ast_special_ref_free(struct AstSpecRef *ref)
{
    ast_node_free(ref->expr);
}

static void ast_special_peek_free(struct AstSpecPeek *peek)
{
    ast_node_free(peek->expr);
}

static void ast_special_poke_free(struct AstSpecPoke *poke)
{
    ast_node_free(poke->reference);
    /* All the nodes in the poke object are linked together. */
}

static void ast_special_begin_free(struct AstSpecBegin *begin)
{
    ast_node_free(begin->collection);
}

static void ast_special_end_free(struct AstSpecEnd *end)
{
    ast_node_free(end->collection);
}

static void ast_special_inc_free(struct AstSpecInc *inc)
{
    ast_node_free(inc->reference);
}

static void ast_special_succ_free(struct AstSpecSucc *succ)
{
    ast_node_free(succ->reference);
}

static void ast_special_free(struct AstSpecial *special)
{
    switch (special->type) {
    case AST_SPEC_DO:
        ast_special_do_free(&special->data.doo);
        break;

    case AST_SPEC_MATCH:
        ast_special_match_free(&special->data.match);
        break;

    case AST_SPEC_IF:
        ast_special_if_free(&special->data.iff);
        break;

    case AST_SPEC_WHILE:
        ast_special_while_free(&special->data.whilee);
        break;

    case AST_SPEC_FUNC_DEF:
        ast_special_func_def_free(&special->data.func_def);
        break;

    case AST_SPEC_AND:
        ast_special_and_free(&special->data.andd);
        break;

    case AST_SPEC_OR:
        ast_special_or_free(&special->data.orr);
        break;

    case AST_SPEC_BIND:
        ast_special_bind_free(&special->data.bind);
        break;

    case AST_SPEC_REF:
        ast_special_ref_free(&special->data.ref);
        break;

    case AST_SPEC_PEEK:
        ast_special_peek_free(&special->data.peek);
        break;

    case AST_SPEC_POKE:
        ast_special_poke_free(&special->data.poke);
        break;

    case AST_SPEC_BEGIN:
        ast_special_begin_free(&special->data.begin);
        break;

    case AST_SPEC_END:
        ast_special_end_free(&special->data.end);
        break;

    case AST_SPEC_INC:
        ast_special_inc_free(&special->data.inc);
        break;

    case AST_SPEC_SUCC:
        ast_special_succ_free(&special->data.succ);
        break;
    }
}

static void ast_func_call_free(struct AstFuncCall *fcall)
{
    ast_node_free(fcall->func);
    /* All the nodes in the function call object are linked together. */
}

static void ast_literal_compound_free(struct AstLiteralCompound *lit_cpd)
{
    ast_node_free(lit_cpd->exprs);
}

static void ast_literal_atomic_free(struct AstLiteralAtomic *lit_atom)
{
    if (lit_atom->type == AST_LIT_ATOM_STRING) {
        mem_free(lit_atom->data.string);
    }
}

void ast_node_free_one(struct AstNode *node)
{
    switch (node->type) {
    case AST_SYMBOL:
        ast_symbol_free(&node->data.symbol);
        break;

    case AST_SPECIAL:
        ast_special_free(&node->data.special);
        break;

    case AST_FUNCTION_CALL:
        ast_func_call_free(&node->data.func_call);
        break;

    case AST_LITERAL_COMPOUND:
        ast_literal_compound_free(&node->data.literal_compound);
        break;

    case AST_LITERAL_ATOMIC:
        ast_literal_atomic_free(&node->data.literal_atomic);
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
