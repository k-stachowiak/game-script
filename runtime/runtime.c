/* Copyright (C) 2014 Krzysztof Stachowiak */

#include <stdlib.h>

#include "stack.h"
#include "eval.h"
#include "error.h"
#include "runtime.h"

static struct Stack *stack;
static struct SymMap sym_map;
static struct AstNode *node_store;

static VAL_LOC_T saved_loc;
static struct AstNode *saved_store;

static void repl_free_bound(void)
{
	if (node_store) {
		ast_node_free(node_store);
	}
	node_store = NULL;
}

void rt_init(void)
{
	stack = stack_make(64 * 1024);
	sym_map_init_global(&sym_map, stack);
	node_store = NULL;
}

void rt_deinit(void)
{
	repl_free_bound();
	sym_map_deinit(&sym_map);
	stack_free(stack);
}

void rt_save(void)
{
	saved_loc = stack->top;
	saved_store = node_store;
}

void rt_restore(void)
{
	while (node_store != saved_store) {
		struct AstNode *temp = node_store;
		ast_node_free_one(temp);
		node_store = node_store->next;
	}

	stack_collapse(stack, saved_loc, stack->top);
}

VAL_LOC_T rt_current_top(void)
{
	return stack->top;
}

void rt_consume_one(struct AstNode *ast, VAL_LOC_T *loc, struct AstNode **next)
{
	if (loc) {
		*loc = stack->top;
	}
	if (next) {
		*next = ast->next;
	}

	eval(ast, stack, &sym_map);

	if (err_state()) {
		return;

	} else if (ast->type == AST_BIND) {
		ast->next = node_store;
		node_store = ast;

	} else {
		stack->top = *loc;
		ast_node_free_one(ast);

	}
}

bool rt_consume_list(struct AstNode *ast_list)
{
	struct AstNode *next;

	rt_save();

	while (ast_list) {
		rt_consume_one(ast_list, NULL, &next);
		if (err_state()) {
			rt_restore();
			/* NOTE: ast_list is used for the iteration so upon error,
			* no nodes that have already been consumed will be freed.
			* They have actually been freed upon success of the consume function.
			*/
			ast_node_free(ast_list);
			return false;
		}
		ast_list = next;
	}
	return true;
}

void rt_for_each_stack_val(void(*f)(VAL_LOC_T, struct Value*))
{
	stack_for_each(stack, f);
}

void rt_for_each_sym(void(*f)(char*, VAL_LOC_T))
{
	sym_map_for_each(&sym_map, f);
}

struct Value rt_peek(VAL_LOC_T location)
{
	return stack_peek_value(stack, location);
}

void rt_print(VAL_LOC_T loc, bool annotate)
{
    val_print(stack, loc, annotate);
}

