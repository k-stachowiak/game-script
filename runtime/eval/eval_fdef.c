/* Copyright (C) 2014,2015 Krzysztof Stachowiak */

#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "collection.h"
#include "eval.h"
#include "eval_detail.h"
#include "error.h"
#include "rt_val.h"

/**
 * Analyzes an AST node to find if it contains children that can potentially
 * contain symbols.
 */
static struct AstNode *efd_get_children(struct AstNode* node)
{
    switch (node->type) {
    case AST_LITERAL:
    case AST_REFERENCE:
        return NULL;

    case AST_DO_BLOCK:
        return node->data.do_block.exprs;

    case AST_BIND:
        return node->data.bind.expr;

    case AST_COMPOUND:
        return node->data.compound.exprs;

    case AST_FUNC_DEF:
        return node->data.func_def.expr;

    case AST_FUNC_CALL:
        /* NOTE tha the func expression is artifically chained with the args list. */
        return node->data.func_call.func;

    case AST_MATCH:
        return node->data.match.values;

    case AST_PARAFUNC:
        return node->data.parafunc.args;
    }

    LOG_ERROR("Unhandled AST node type.\n");
    exit(1);
}

/**
 * Checks whether a given AST node contains a reference to an external symbol.
 * If it does, then true is returned and the "symbol" argument is set to the
 * said symbol value. Otherwise false is returned.
 */
static bool efd_refers_to_symbol(struct AstNode *node, char **symbol)
{
    if (node->type == AST_REFERENCE) {
        *symbol = node->data.reference.symbol;
        return true;

    } else {
        return false;

    }
}

/**
 * Simple wrapper around the non global symbol lookup.
 * As a side effect, upon success the location is stored in the loc argument.
 */
static bool efd_is_non_global(char *symbol, struct SymMap *sym_map, VAL_LOC_T *loc)
{
    struct SymMapNode *smn = sym_map_find_not_global(sym_map, symbol);
    if (smn) {
        *loc = smn->stack_loc;
        return true;
    } else {
        return false;
    }
}

/**
 * Searches recursively for all symbols in the given function definitions.
 * Pushes on the stack all the values refered to that are not global and
 * not passed in as an argument.
 */
static int efd_push_captures_rec(
        struct Stack *stack,
        struct SymMap *sym_map,
        struct AstFuncDef *func_def,
        struct AstNode *node)
{
    char *symbol;
    VAL_LOC_T cap_location;
    int captures = 0;
    struct AstNode *child = efd_get_children(node);

    /* Store a capture from the current node if necessary and possible. */
    if (efd_refers_to_symbol(node, &symbol) &&
            !pattern_list_contains_symbol(func_def->formal_args, symbol) &&
            efd_is_non_global(symbol, sym_map, &cap_location)) {
        rt_val_push_func_cap(stack, symbol, cap_location);
        ++captures;
    }

    /* Analyze children of the current node. */
    while (child) {
        captures += efd_push_captures_rec(stack, sym_map, func_def, child);
        child = child->next;
    }

    return captures;
}

static void efd_push_captures(
        struct Stack *stack,
        struct SymMap *sym_map,
        struct AstFuncDef *func_def)
{
    VAL_LOC_T cap_count_loc;
    VAL_SIZE_T cap_count = 0;
    rt_val_push_func_cap_init_deferred(stack, &cap_count_loc);
    cap_count = efd_push_captures_rec(stack, sym_map, func_def, func_def->expr);
    rt_val_push_func_cap_final_deferred(stack, cap_count_loc, cap_count);
}

void eval_func_def(
        struct AstNode *node,
        struct Stack *stack,
        struct SymMap *sym_map)
{
    VAL_LOC_T size_loc, data_begin;
    VAL_SIZE_T arity = node->data.func_def.arg_count;
    rt_val_push_func_init(stack, &size_loc, &data_begin, arity, VAL_FUNC_AST, (void*)node);
    efd_push_captures(stack, sym_map, &node->data.func_def);
    rt_val_push_func_appl_init(stack, 0);
    rt_val_push_func_final(stack, size_loc, data_begin);
}

