/* Copyright (C) 2014 Krzysztof Stachowiak */

#include <stdlib.h>

#include "stack.h"
#include "eval.h"
#include "error.h"
#include "repl_state.h"

static struct Stack *stack;
static struct SymMap sym_map;
static VAL_LOC_T saved_loc;

static struct {
	struct AstNode **data;
	int size, cap;
} stored_nodes;

static void repl_free_bound(void)
{
	while (stored_nodes.size) {
		ast_node_free(stored_nodes.data[0]);
		ARRAY_REMOVE(stored_nodes, 0);
	}
}

void repl_state_init(void)
{
	stack = stack_make(1024);
	sym_map_init(&sym_map, NULL, stack);
}

void repl_state_deinit(void)
{
	repl_free_bound();
	sym_map_deinit(&sym_map);
	stack_free(stack);
}

void repl_state_save(void)
{
	saved_loc = stack->top;
}

void repl_state_restore(void)
{
	/* TODO: Don't leak stored_nodes here! */
	stack_collapse(stack, saved_loc, stack->top);
}

VAL_LOC_T repl_state_consume(struct AstNode *ast)
{
	VAL_LOC_T location = stack->top;
	eval(ast, stack, &sym_map);

	if (!err_state() && ast->type == AST_BIND) {
		ARRAY_APPEND(stored_nodes, ast);
	} else {
		ast_node_free(ast);
	}

	return location;
}

void repl_state_for_each_stack_val(void(*f)(VAL_LOC_T, struct Value*))
{
	stack_for_each(stack, f);
}

void repl_state_for_each_sym(void(*f)(char*, VAL_LOC_T))
{
	sym_map_for_each(&sym_map, f);
}

struct Value repl_state_peek(VAL_LOC_T location)
{
	return stack_peek_value(stack, location);
}
