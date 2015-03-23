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
	case AST_BIF:
	case AST_LITERAL:
	case AST_REFERENCE:
	case AST_PATTERN:
		return NULL;

	case AST_DO_BLOCK:
		return node->data.do_block.exprs;

	case AST_BIND:
		return node->data.bind.expr;

	case AST_IFF:
		return node->data.iff.test; /* Points to branches too. */

	case AST_COMPOUND:
		return node->data.compound.exprs;

	case AST_FUNC_DEF:
		return node->data.func_def.expr;

	case AST_FUNC_CALL:
		return node->data.func_call.actual_args;

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
static bool efd_has_symbol(struct AstNode *node, char **symbol)
{
	switch (node->type) {
	case AST_DO_BLOCK:
	case AST_BIF:
	case AST_LITERAL:
	case AST_COMPOUND:
	case AST_BIND:
	case AST_IFF:
	case AST_FUNC_DEF:
	case AST_PATTERN:
    case AST_PARAFUNC: /* don't capture parafunc symbol */
		return false;

	case AST_REFERENCE:
		*symbol = node->data.reference.symbol;
		return true;

	case AST_FUNC_CALL:
		*symbol = node->data.func_call.symbol;
		return true;
	}

	LOG_ERROR("Unhandled AST node type.\n");
	exit(1);
}

/** Checks whether symbol is defined in the symbol map. */
static bool efd_is_defined(char *symbol, struct SymMap *sym_map)
{
	return (bool)(sym_map_find(sym_map, symbol));
}

/**
 * Simple wrapper around the non global symbol lookup.
 * As a side effect, upon success the location is stored in the loc argument.
 */
static bool efd_is_global(char *symbol, struct SymMap *sym_map, VAL_LOC_T *loc)
{
	struct SymMapKvp *kvp = sym_map_find(sym_map, symbol);

	if (kvp) {
		*loc = kvp->stack_loc;
	}

	return kvp != sym_map_find_not_global(sym_map, symbol);
}

/** Simple wrapper around an argument list lookup. */
static bool efd_is_argument(char *symbol, struct AstCommonFunc *func)
{
	int i;
	for (i = 0; i < func->arg_count; ++i) {
		if (strcmp(symbol, func->formal_args[i]) == 0) {
			return true;
		}
	}
	return false;
}

/**
 * Searches recursively for all symbols in the given function definitions.
 * Pushes on the stack all the values refered to that are not global and
 * not passed in as an argument.
 */
static void efd_push_captures(
        struct Stack *stack,
        struct SymMap *sym_map,
		struct AstFuncDef *func_def)
{
	VAL_LOC_T cap_count_loc;
	VAL_SIZE_T cap_count = 0;

	rt_val_push_func_cap_init_deferred(stack, &cap_count_loc);

	struct { struct AstNode **data; int size, cap; } to_visit = { 0 };
	ARRAY_APPEND(to_visit, func_def->expr);

	while (to_visit.size > 0) {

		struct AstNode *node = to_visit.data[0];
		struct AstNode *to_append;
		VAL_LOC_T cap_location;

		char *symbol;

		if ((to_append = efd_get_children(node))) {
			ARRAY_APPEND(to_visit, to_append);
		}

		if (efd_has_symbol(node, &symbol) &&
			!efd_is_global(symbol, sym_map, &cap_location) &&
			!efd_is_argument(symbol, &func_def->func)) {
				if (!efd_is_defined(symbol, sym_map)) {
					if (node->type != AST_FUNC_CALL) {
						/* Note: it is allowed for the func call symbol not to be defined.
                         * in such case it shall not be captured in the closure and
                         * will result in runtime error if called before the definition
                         * is made.
                         */
						eval_error_not_found(symbol);
					}
					return;
				}
				rt_val_push_func_cap(stack, symbol, cap_location);
				++cap_count;
		}

		ARRAY_REMOVE(to_visit, 0);
	}

	rt_val_push_func_cap_final_deferred(stack, cap_count_loc, cap_count);
	ARRAY_FREE(to_visit);
}

void eval_func_def(
		struct AstNode *node,
        struct Stack *stack,
        struct SymMap *sym_map)
{
	VAL_LOC_T size_loc, data_begin;
	VAL_SIZE_T arity = node->data.func_def.func.arg_count;
	rt_val_push_func_init(stack, &size_loc, &data_begin, arity, (void*)node, NULL);
	efd_push_captures(stack, sym_map, &node->data.func_def);
	if (err_state()) {
		return;
	}
	rt_val_push_func_appl_init(stack, 0);
	rt_val_push_func_final(stack, size_loc, data_begin);
}

