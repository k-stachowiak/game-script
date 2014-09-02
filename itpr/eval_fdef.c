/* Copyright (C) 2014 Krzysztof Stachowiak */

#include <stdlib.h>
#include <string.h>

#include "eval.h"
#include "value.h"
#include "common.h"

static VAL_HEAD_SIZE_T zero = 0;

/**
 * Pushes incomplete function value header.
 * Returns the location of the size to be filled later on.
 */
static VAL_LOC_T efd_push_header(struct Stack *stack)
{
	static VAL_HEAD_TYPE_T type = (VAL_HEAD_TYPE_T)VAL_FUNCTION;
	stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type);
	return stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&zero);
}

/**
 * Pushes the binary representation of the pointer to the AST node
 * implementing function to the stack.
 * Returns the location of the beginning of the addres, i.e. beginning
 * of the entire data block for the function value.
 */
static VAL_LOC_T efd_push_impl_prt(struct AstNode* impl, struct Stack *stack)
{
	void *impl_address = (void*)impl;
	return stack_push(stack, VAL_PTR_BYTES, (char*)&impl_address);
}

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
		return NULL;

	case AST_BIND:
		return node->data.bind.expr;

	case AST_IFF:
		return node->data.iff.test; /* Points to branches too. */

	case AST_COMPOUND:
		return node->data.compound.exprs;

	case AST_FUNC_DEF:
		return node->data.func_def.exprs;

	case AST_FUNC_CALL:
		return node->data.func_call.actual_args;
	}

	printf("Unhandled AST node type.\n");
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
	case AST_BIF:
	case AST_LITERAL:
	case AST_COMPOUND:
	case AST_BIND:
	case AST_IFF:
	case AST_FUNC_DEF:
		return false;

	case AST_REFERENCE:
		*symbol = node->data.reference.symbol;
		return true;

	case AST_FUNC_CALL:
		*symbol = node->data.func_call.symbol;
		return true;
	}

	printf("Unhandled AST node type.\n");
	exit(1);
}

/**
 * Simple wrapper around the non global symbol lookup.
 * As a side effect, upon success the location is stored in the loc argument.
 */
static bool efd_is_global(char *symbol, struct SymMap *sym_map, VAL_LOC_T *loc)
{
	struct SymMapKvp *kvp = sym_map_find(sym_map, symbol);

	if (kvp) {
		*loc = kvp->location;
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

/** Copies a value from the given location to the top of the stack. */
void efd_copy_capture(struct Stack *stack, char *symbol, VAL_LOC_T location)
{
	VAL_SIZE_T len = strlen(symbol);
	struct ValueHeader header = stack_peek_header(stack, location);
	stack_push(stack, VAL_SIZE_BYTES, (char*)&len);
	stack_push(stack, len, symbol);
	stack_push(stack, header.size + VAL_HEAD_BYTES, stack->buffer + location);
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
	VAL_LOC_T cap_count_loc = stack_push(stack, VAL_SIZE_BYTES, (char*)&zero);
	VAL_SIZE_T cap_count = 0;

	struct { struct AstNode **data; int size, cap; } to_visit = { 0 };
	ARRAY_APPEND(to_visit, func_def->exprs);

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
				efd_copy_capture(stack, symbol, cap_location);
				++cap_count;
		}

		ARRAY_REMOVE(to_visit, 0);
	}

	memcpy(stack->buffer + cap_count_loc, &cap_count, VAL_SIZE_BYTES);
	ARRAY_FREE(to_visit);
}

void eval_func_def(
		struct AstNode *node,
		struct Stack *stack,
		struct SymMap *sym_map)
{
	VAL_SIZE_T data_size;

	VAL_LOC_T size_loc = efd_push_header(stack);
	VAL_LOC_T data_begin = efd_push_impl_prt(node, stack);
	efd_push_captures(stack, sym_map, &node->data.func_def);
	stack_push(stack, VAL_SIZE_BYTES, (char*)&zero); /* No applied args. */

	data_size = stack->top - data_begin;
	memcpy(stack->buffer + size_loc, &data_size, VAL_HEAD_SIZE_BYTES);
}
