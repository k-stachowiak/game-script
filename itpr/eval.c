/* Copyright (C) 2014 Krzysztof Stachowiak */

#include <string.h>
#include <stdlib.h>

#include "eval.h"
#include "eval_detail.h"
#include "value.h"
#include "bif.h"
#include "error.h"

static void eval_compound(
        struct AstNode *node,
        struct Stack *stack,
        struct SymMap *sym_map)
{
    static VAL_HEAD_SIZE_T zero = 0;
    VAL_HEAD_TYPE_T type;
    VAL_LOC_T size_loc, data_begin, data_size;
    struct AstNode *current = node->data.compound.exprs;

    switch (node->data.compound.type) {
        case AST_CPD_ARRAY:
            type = (VAL_HEAD_TYPE_T)VAL_ARRAY;
            break;

        case AST_CPD_TUPLE:
            type = (VAL_HEAD_TYPE_T)VAL_TUPLE;
            break;
    }

    /* Header. */
	stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type);
    size_loc = stack_push(stack, VAL_SIZE_BYTES, (char*)&zero);

    /* Data. */
    data_begin = stack->top;
    while (current) {
        eval_impl(current, stack, sym_map);
        if (err_state()) {
            return;
        }
        current = current->next;
    }

    /* Hack value size to correct value. */
    data_size = stack->top - data_begin;
    memcpy(stack->buffer + size_loc, &data_size, VAL_HEAD_SIZE_BYTES);
}

static void eval_literal(struct AstNode *node, struct Stack *stack)
{
    VAL_HEAD_TYPE_T type;
    VAL_HEAD_SIZE_T size;

	/* Bring everything down to the values of the interpreter type.
	 * This will be performed by the C compiler conversions from
	 * whatever resides in the AST node object into the local variables
	 * declared below. String is an exception as char* is general enough.
	 */
	VAL_BOOL_T boolean;
	VAL_CHAR_T character;
	VAL_INT_T integer;
	VAL_REAL_T real;

    char *value = NULL;

    switch (node->data.literal.type) {
    case AST_LIT_BOOL:
        type = (VAL_HEAD_TYPE_T)VAL_BOOL;
        size = VAL_BOOL_BYTES;
		boolean = node->data.literal.data.boolean;
        value = (char*)&boolean;
        break;

    case AST_LIT_CHAR:
        type = (VAL_HEAD_TYPE_T)VAL_CHAR;
        size = VAL_CHAR_BYTES;
		character = node->data.literal.data.character;
		value = (char*)&character;
        break;

    case AST_LIT_INT:
        type = (VAL_HEAD_TYPE_T)VAL_INT;
        size = VAL_INT_BYTES;
		integer = node->data.literal.data.integer;
        value = (char*)&integer;
        break;

    case AST_LIT_REAL:
        type = (VAL_HEAD_TYPE_T)VAL_REAL;
        size = VAL_REAL_BYTES;
		real = node->data.literal.data.real;
        value = (char*)&real;
        break;

    case AST_LIT_STRING:        
        type = (VAL_HEAD_TYPE_T)VAL_STRING;
		size = strlen(node->data.literal.data.string);
		value = node->data.literal.data.string;
        break;
    }

	stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type);
	stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&size);
	stack_push(stack, size, value);
}

static void eval_bind(
		struct AstNode *node,
		struct Stack *stack,
		struct SymMap *sym_map)
{
    VAL_LOC_T location = eval_impl(node->data.bind.expr, stack, sym_map);
    if (!err_state()) {
        sym_map_insert(sym_map, node->data.bind.symbol, location);
    }
}

static void eval_iff(
		struct AstNode *node,
		struct Stack *stack,
		struct SymMap *sym_map)
{
	VAL_LOC_T test_loc, temp_begin, temp_end;
	struct Value test_val;

	temp_begin = stack->top;
	test_loc = eval_impl(node->data.iff.test, stack, sym_map);
	test_val = stack_peek_value(stack, test_loc);
	temp_end = stack->top;

	if ((enum ValueType)test_val.header.type != VAL_BOOL) {
		err_set(ERR_EVAL, "Text expression isn't a boolean value.");
		return;
	}

	if (test_val.primitive.boolean) {
		eval_impl(node->data.iff.true_expr, stack, sym_map);
	} else {
		eval_impl(node->data.iff.false_expr, stack, sym_map);
	}

	stack_collapse(stack, temp_begin, temp_end);
}

static void eval_reference(
		struct AstNode *node,
		struct Stack *stack,
		struct SymMap *sym_map)
{
	struct SymMapKvp *kvp;
	struct ValueHeader header;
	VAL_HEAD_SIZE_T size;

	if (!(kvp = sym_map_find(sym_map, node->data.reference.symbol))) {
	    err_set(ERR_EVAL, "Symbol not found.");
		return;
	}

	header = stack_peek_header(stack, kvp->location);
	size = header.size + VAL_HEAD_BYTES;
	stack_push(stack, size, stack->buffer + kvp->location);
}

/**
 * Note: This is not called during regular evaluation, but when 
 *       BIFs are added to the global scope.
 */
static void eval_bif(struct AstNode *node, struct Stack *stack)
{
    static VAL_HEAD_TYPE_T type = (VAL_HEAD_TYPE_T)VAL_FUNCTION;
    static VAL_HEAD_SIZE_T size = VAL_PTR_BYTES + 2 * VAL_SIZE_BYTES;
    static VAL_HEAD_SIZE_T zero = 0;
    void* impl = (void*)node;

	stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type);
	stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&size);
	stack_push(stack, VAL_PTR_BYTES, (char*)&impl);
	stack_push(stack, VAL_SIZE_BYTES, (char*)&zero);
	stack_push(stack, VAL_SIZE_BYTES, (char*)&zero);    
}

/* Main evaluation dispatch.
 * =========================
 */

VAL_LOC_T eval_impl(
		struct AstNode *node,
		struct Stack *stack,
		struct SymMap *sym_map)
{
    VAL_LOC_T begin = stack->top;

    switch (node->type) {
    case AST_LITERAL:
		LOG_DEBUG("Evaluating literal");
        eval_literal(node, stack);
        break;

    case AST_COMPOUND:
		LOG_DEBUG("Evaluating compound %s", node->data.compound.type == AST_CPD_ARRAY ? "array" : "tuple");
        eval_compound(node, stack, sym_map);
        break;

    case AST_BIND:
		LOG_DEBUG("Evaluating bind(%s)", node->data.bind.symbol);
    	eval_bind(node, stack, sym_map);
    	break;

	case AST_IFF:
		LOG_DEBUG("Evaluating if");
		eval_iff(node, stack, sym_map);
		break;

    case AST_REFERENCE:
		LOG_DEBUG("Evaluating reference(%s)", node->data.reference.symbol);
    	eval_reference(node, stack, sym_map);
    	break;

    case AST_FUNC_DEF:
		LOG_DEBUG("Evaluating function definition (%d)", node->data.func_def.func.arg_count);
        eval_func_def(node, stack, sym_map);
        break;

    case AST_BIF:
		LOG_DEBUG("Evaluating bif");
        eval_bif(node, stack);
        break;

    case AST_FUNC_CALL:
		LOG_DEBUG("Evaluating function call (%s)", node->data.func_call.symbol);
        eval_func_call(node, stack, sym_map);
        break;

	default:
		printf("Unhandled AST node type.\n");
		exit(1);
    }

    if (err_state()) {
        return -1;
    } else {
        return begin;
    }
}

VAL_LOC_T eval(
        struct AstNode *node,
        struct Stack *stack,
        struct SymMap *sym_map)
{
    VAL_LOC_T begin, result, end;

    err_reset();

    begin = stack->top;
    result = eval_impl(node, stack, sym_map);
    end = stack->top;

    if (!err_state()) {
        return result;
    } else {
        stack_collapse(stack, begin, end);
        return -1;
    }
}
