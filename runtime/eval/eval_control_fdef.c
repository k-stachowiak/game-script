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
    struct AstControl *control;
    switch (node->type) {
    case AST_CONTROL:
        control = &node->data.control;
        switch(control->type) {
        case AST_CTL_REFERENCE:
            return NULL;

        case AST_CTL_DO:
            return control->data.doo.exprs;

        case AST_CTL_BIND:
            return control->data.bind.expr;

        case AST_CTL_MATCH:
            return control->data.match.values;

        case AST_CTL_FUNC_DEF:
            return control->data.fdef.expr;

        case AST_CTL_FUNC_CALL:
            /* NOTE tha the func expression is artifically chained with the args list. */
            return control->data.fcall.func;
        }

    case AST_SPECIAL:
        return node->data.special.args;

    case AST_COMPOUND:
        return node->data.compound.exprs;

    case AST_LITERAL:
        return NULL;
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
    if (node->type == AST_CONTROL) {
        struct AstControl *control = &node->data.control;
        if (control->type == AST_CTL_REFERENCE) {
            *symbol = control->data.reference.symbol;
            return true;
        }
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
        struct AstCtlFuncDef *func_def,
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
        struct Stack *stack,
        struct SymMap *sym_map,
        struct AstCtlFuncDef *func_def)
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

void eval_func_def(
        struct AstCtlFuncDef *fdef,
        struct Runtime *rt,
        struct SymMap *sym_map)
{
    VAL_LOC_T size_loc, data_begin;
    VAL_SIZE_T arity = fdef->arg_count;
    rt_val_push_func_init(&rt->stack, &size_loc, &data_begin, arity, VAL_FUNC_AST, (void*)fdef);
    efd_push_captures(&rt->stack, sym_map, fdef);
    rt_val_push_func_appl_init(&rt->stack, 0);
    rt_val_push_func_final(&rt->stack, size_loc, data_begin);
}

