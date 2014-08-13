/* Copyright (C) 2014 Krzysztof Stachowiak */

#include <stdlib.h>

#include "ast.h"
#include "itpr.h"
#include "function_call.h"

static int call_function_arity(struct Value *val)
{
	/* TODO: Change from AST to a functio type. */
	struct AstNode *def = val->function.def;
	int applied_args = val->function.applied.size;
	int formal_args;

	switch (def->type) {
	case AST_BIF:
		formal_args = def->data.bif.func.arg_count;
		break;
	case AST_FUNC_DEF:
		formal_args = def->data.func_def.func.arg_count;
		break;
	default:
		printf("Illegal ast node type in funcion call.\n");
		exit(1);
	}

	return formal_args - applied_args;
}

static ptrdiff_t curry_on(
		struct Stack *stack,
		struct SymMap *sym_map,
		struct Value val,
		struct AstNode *call_node)
{
	int i;
	static uint32_t function_type = (uint32_t)VAL_FUNCTION;
	auto   uint32_t size;
	ptrdiff_t begin = stack->top;
	ptrdiff_t size_location;
	uint32_t captures_count;
	uint32_t applied_count;
	struct AstNode *arg;

	/* Push header. */
	stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&function_type);
	size_location = stack->top;
	stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&size);

	/* Push definition pointer. */
	stack_push(stack, VAL_PTR_BYTES, (char*)val->function.def);

	/* Push captures. */
	captures_count = val->function.captures.size;
	stack_push(stack, VAL_COMMON_SIZE_BYTES, (char*)captures_count);
	for (i = 0; i < val->function.captures.size; ++i) {
		struct Capture *capture = val->function.captures.data + i;
		uint32_t symbol_len = strlen(capture->symbol);
		uint32_t line = capture->loc.line;
		uint32_t column = capture->loc.column;
		stack_push(stack, symbol_len, capture->symbol);
		stack_push(stack, VAL_COMMON_SIZE_BYTES, (char*)line);
		stack_push(stack, VAL_COMMON_SIZE_BYTES, (char*)column);
	}

	/* Push applied. */
	applied_count = val->function.applied.size;
	for (i = 0; i < val->function.applied.size; ++i) {

	}

	for (arg = call_node->data.func_call.actual_args; arg; arg = arg->next) {
		ptrdiff_t arg_location = eval(arg, stack, sym_map);
	}

	return begin;
}

void call_function(
	struct Stack *stack,
	struct SymMap *sym_map,
	struct Value val,
	struct AstNode *call_node)
{
	(void)stack;
	(void)sym_map;

	int arity = call_function_arity(&val);
	int applied_count = ast_list_len(call_node->data.func_call.actual_args);

	if (applied_count > arity) {
		/* TODO: Formalize runtime errors. */
		printf("Too many arguments applied.\n");
		exit(1);
	}

	if (applied_count < arity) {
		curry_on(stack, sym_map, val, call_node);
	}

	/*
	 * 1. Validate arity against passed arguments.
	 * 2. Append applied arguments to the applied list in function value.
	 * 3. a) Return if currying only.
	 * 3. b) Call function if args ready:
	 * 4. 1. add captured values to scope,
	 * 4. 2. add applied arguments to scope,l
	 * 4. 3. evaluate function expression with new scope.
	 */
}
