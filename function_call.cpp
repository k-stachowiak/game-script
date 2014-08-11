/* Copyright (C) 2014 Krzysztof Stachowiak */

#include "function_call.h"

ptrdiff_t call_function(
	struct Stack *stack,
	struct SymMap *sym_map,
	struct Value val,
	struct AstNode *node)
{
	/*
	 * 1. Validate arity against passed arguments.
	 * 2. Append applied arguments to the applied list in function value.
	 * 3. a) Return if currying only.
	 * 3. b) Call function if args ready:
	 * 4. 1. add captured values to scope,
	 * 4. 2. add applied arguments to scope,
	 * 4. 3. evaluate function expression with new scope.
	 */
	return -1;
}
