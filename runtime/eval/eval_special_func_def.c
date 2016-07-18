/* Copyright (C) 2015 Krzysztof Stachowiak */

#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "log.h"
#include "symmap.h"
#include "collection.h"

struct CaptureCandidate {
    char *symbol;
    int lookup_depth;
    VAL_LOC_T stack_loc;
};

struct CaptureCandidateArray {
    struct CaptureCandidate *data;
    int cap, size;
};

/**
 * Analyzes an AST node to find if it contains children that can potentially
 * contain symbols.
 */
static struct AstNode *efd_get_children(struct AstNode* node)
{
    struct AstSpecial *special;

    switch (node->type) {
    case AST_SYMBOL:
        return NULL;

    case AST_SPECIAL:
        special = &node->data.special;
        switch (special->type) {
        case AST_SPEC_DO:
            return special->data.doo.exprs;

        case AST_SPEC_MATCH:
            return special->data.match.expr;

        case AST_SPEC_IF:
            return special->data.iff.test;

        case AST_SPEC_WHILE:
            return special->data.whilee.test;

        case AST_SPEC_FUNC_DEF:
            return special->data.func_def.expr;

        case AST_SPEC_AND:
            return special->data.andd.exprs;

        case AST_SPEC_OR:
            return special->data.orr.exprs;

        case AST_SPEC_BIND:
            return special->data.bind.expr;

        case AST_SPEC_REF:
            return special->data.ref.expr;

        case AST_SPEC_PEEK:
            return special->data.peek.expr;

        case AST_SPEC_POKE:
            return special->data.poke.reference;

        case AST_SPEC_BEGIN:
            return special->data.begin.collection;

        case AST_SPEC_END:
            return special->data.end.collection;

        case AST_SPEC_INC:
            return special->data.inc.reference;

        case AST_SPEC_SUCC:
            return special->data.succ.reference;
        }
        LOG_ERROR("Unhandled special AST node type.");
        exit(1);

    case AST_FUNCTION_CALL:
        return node->data.func_call.func;

    case AST_LITERAL_COMPOUND:
        return node->data.literal_compound.exprs;

    case AST_LITERAL_ATOMIC:
        return NULL;
    }

    LOG_ERROR("Unhandled AST node type.");
    exit(1);
}

/**
 * Checks whether a given AST node contains a reference to an external symbol.
 * If it does, then true is returned and the "symbol" argument is set to the
 * said symbol value. Otherwise false is returned.
 */
static bool efd_refers_to_symbol(struct AstNode *node, char **symbol)
{
    if (node->type == AST_SYMBOL) {
        *symbol = node->data.symbol.symbol;
        return true;
    }
    return false;
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
static void efd_push_captures_rec(
        int level,
        struct Stack *stack,
        struct SymMap *sym_map,
        struct AstSpecFuncDef *func_def,
        struct AstNode *node,
        struct CaptureCandidateArray *capture_candidates)
{
    char *symbol;
    VAL_LOC_T cap_location;
    struct AstNode *child = efd_get_children(node);

    /* Store a capture from the current node if necessary and possible. */
    if (efd_refers_to_symbol(node, &symbol) &&
            !pattern_list_contains_symbol(func_def->formal_args, symbol) &&
            efd_is_non_global(symbol, sym_map, &cap_location)) {
        struct CaptureCandidate capture_candidate = { symbol, level, cap_location };
        ARRAY_APPEND(*capture_candidates, capture_candidate);
    }

    /* Analyze children of the current node. */
    while (child) {
        efd_push_captures_rec(level + 1, stack, sym_map, func_def, child, capture_candidates);
        child = child->next;
    }
}

static void efd_push_captures(
        struct AstSpecFuncDef* func_def,
        struct Stack *stack,
        struct SymMap *sym_map)
{
    VAL_LOC_T cap_count_loc;
    int i, j, cap_count;
    struct CaptureCandidate *candidate;
    struct CaptureCandidateArray capture_candidates = { NULL, 0, 0 };

    /* 1. Initialize the capture push operation */
    rt_val_push_func_cap_init_deferred(stack, &cap_count_loc);

    /* 2. Store all refered symbols, a.k.a. capture candidates */
    efd_push_captures_rec(0, stack, sym_map, func_def, func_def->expr, &capture_candidates);

    /* 3. Filter out the duplicate, discarding the deeper ones */
    for (i = 0; i < capture_candidates.size; ++i) {
        for (j = i + 1; j < capture_candidates.size; ++j) {

            /* 3.1. Store helper references to the compared objects */
            struct CaptureCandidate *x = capture_candidates.data + i;
            struct CaptureCandidate *y = capture_candidates.data + j;

            /* 3.2. Skip pairs with different symbols */
            if (strcmp(x->symbol, y->symbol) != 0) {
                continue;
            } else {
                LOG_TRACE("Duplicate found while pushing captures: %s", x->symbol);
            }

            /* 3.3. If x is to be discarded, overwrite it with the y value */
            if (x->lookup_depth > y->lookup_depth) {
                *x = *y;
            }

            /* 3.4. By now the item under the index j (a.k.a. y) is obsolete */
            ARRAY_REMOVE(capture_candidates, j);
            --j;
        }
    }

    /* 4. Push the actual captures in the setack */
    candidate = capture_candidates.data;
    cap_count = capture_candidates.size;
    for (i = 0; i < cap_count; ++i) {
        rt_val_push_func_cap(stack, candidate->symbol, candidate->stack_loc);
        ++candidate;
    }
    mem_free(capture_candidates.data);

    /* 5. Finalize the capture push with updating the actual captures count */
    rt_val_push_func_cap_final_deferred(stack, cap_count_loc, cap_count);
}

void eval_special_func_def(
        struct AstSpecFuncDef* func_def,
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct SourceLocation *src_loc)
{
    VAL_LOC_T size_loc, data_begin;
    VAL_SIZE_T arity = func_def->arg_count;
    (void)src_loc;
    rt_val_push_func_init(&rt->stack, &size_loc, &data_begin, arity, VAL_FUNC_AST, (void*)func_def);
    efd_push_captures(func_def, &rt->stack, sym_map);
    rt_val_push_func_appl_init(&rt->stack, 0);
    rt_val_push_func_final(&rt->stack, size_loc, data_begin);
}

