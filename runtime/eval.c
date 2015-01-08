/* Copyright (C) 2014,2015 Krzysztof Stachowiak */

#include <string.h>
#include <stdlib.h>

#include "log.h"
#include "collection.h"
#include "eval.h"
#include "eval_detail.h"
#include "bif.h"
#include "error.h"
#include "runtime.h"
#include "rt_val.h"

struct {
	struct SourceLocation *data;
	int size, cap;
} location_stack = { NULL, 0, 0 };

void eval_location_reset(void)
{
	ARRAY_FREE(location_stack);
}

void eval_location_push(struct SourceLocation *loc)
{
	ARRAY_APPEND(location_stack, *loc);
}

void eval_location_pop(void)
{
	ARRAY_POP(location_stack);
}

struct SourceLocation *eval_location_top(void)
{
	return location_stack.data + location_stack.size - 1;
}

static void eval_common_error(char *issue)
{
	struct ErrMessage msg;
	err_msg_init_src(&msg, "EVAL", eval_location_top());
	err_msg_append(&msg, "%s", issue);
	err_msg_set(&msg);
}

/* NOTE: this is not static as it is shared in other modules. */
void eval_error_not_found(char *symbol)
{
	struct ErrMessage msg;
	err_msg_init_src(&msg, "EVAL", eval_location_top());
	err_msg_append(&msg, "Symbol \"%s\" not found", symbol);
	err_msg_set(&msg);
}

static void eval_compound(
        struct AstNode *node,
        struct Runtime *rt,
        struct SymMap *sym_map)
{
    VAL_LOC_T size_loc, data_begin, data_size;
    struct AstNode *current = node->data.compound.exprs;

	/* Header. */
    switch (node->data.compound.type) {
        case AST_CPD_ARRAY:
			rt_val_push_array_init(rt->stack, &size_loc);
            break;

        case AST_CPD_TUPLE:
			rt_val_push_tuple_init(rt->stack, &size_loc);
            break;
    }

    /* Data. */
    data_begin = rt->stack->top;
    while (current) {
        eval_impl(current, rt, sym_map);
        if (err_state()) {
            return;
        }
        current = current->next;
    }

    /* Hack value size to correct value. */
    data_size = rt->stack->top - data_begin;
	rt_val_push_cpd_final(rt->stack, size_loc, data_size);
}

static void eval_literal(struct AstNode *node, struct Stack *stack)
{
    switch (node->data.literal.type) {
    case AST_LIT_BOOL:
		rt_val_push_bool(stack, node->data.literal.data.boolean);
        break;

    case AST_LIT_CHAR:
		rt_val_push_char(stack, node->data.literal.data.character);
        break;

    case AST_LIT_INT:
		rt_val_push_int(stack, node->data.literal.data.integer);
        break;

	case AST_LIT_REAL:
		rt_val_push_real(stack, node->data.literal.data.real);
        break;

	case AST_LIT_STRING:
		rt_val_push_string(stack, node->data.literal.data.string);
        break;
    }
}

static void eval_do_block(
		struct AstNode *node,
		struct Runtime *rt,
		struct SymMap *sym_map)
{
	struct AstNode *expr = node->data.do_block.exprs;
	VAL_LOC_T begin = rt->stack->top;
	VAL_LOC_T end = rt->stack->top;
	for (; expr; expr = expr->next) {
		end = eval_impl(expr, rt, sym_map);
		if (err_state()) {
			break;
		}
	}
	stack_collapse(rt->stack, begin, end);
}

static void eval_bind(
		struct AstNode *node,
		struct Runtime *rt,
		struct SymMap *sym_map)
{
	struct AstNode *expr = node->data.bind.expr;
    VAL_LOC_T location = eval_impl(expr, rt, sym_map);
    if (!err_state()) {
        sym_map_insert(sym_map, node->data.bind.symbol, location, &expr->loc);
    }
}

static void eval_iff(
		struct AstNode *node,
		struct Runtime *rt,
		struct SymMap *sym_map)
{
	VAL_LOC_T test_loc, temp_begin, temp_end;
	VAL_BOOL_T test_val;

	temp_begin = rt->stack->top;
	test_loc = eval_impl(node->data.iff.test, rt, sym_map);
	temp_end = rt->stack->top;

    if (rt_val_peek_type(rt, test_loc) != VAL_BOOL) {
		eval_common_error("Test expression isn't a boolean value.");
        stack_collapse(rt->stack, temp_begin, temp_end);
		return;
    }

	test_val = rt_val_peek_bool(rt, test_loc);
    stack_collapse(rt->stack, temp_begin, temp_end);

	if (test_val) {
		eval_impl(node->data.iff.true_expr, rt, sym_map);
	} else {
		eval_impl(node->data.iff.false_expr, rt, sym_map);
	}
}

static void eval_reference(
		struct AstNode *node,
		struct Stack *stack,
		struct SymMap *sym_map)
{
	struct SymMapKvp *kvp;
	char *symbol = node->data.reference.symbol;

	if (!(kvp = sym_map_find(sym_map, symbol))) {
		eval_error_not_found(symbol);
		return;
	}

	stack_push_copy(stack, kvp->stack_loc);
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
		struct Runtime *rt,
		struct SymMap *sym_map)
{
    VAL_LOC_T begin = rt->stack->top;
	eval_location_push(&node->loc);

    switch (node->type) {
    case AST_COMPOUND:
        eval_compound(node, rt, sym_map);
        break;

    case AST_LITERAL:
        eval_literal(node, rt->stack);
        break;

	case AST_DO_BLOCK:
		eval_do_block(node, rt, sym_map);
		break;

    case AST_BIND:
    	eval_bind(node, rt, sym_map);
    	break;

	case AST_IFF:
		eval_iff(node, rt, sym_map);
		break;

    case AST_REFERENCE:
    	eval_reference(node, rt->stack, sym_map);
    	break;

    case AST_FUNC_DEF:
        eval_func_def(node, rt->stack, sym_map);
        break;

    case AST_BIF:
        eval_bif(node, rt->stack);
        break;

    case AST_FUNC_CALL:
        eval_func_call(node, rt, sym_map);
        break;

	default:
		LOG_ERROR("Unhandled AST node type.\n");
		exit(1);
    }

	eval_location_pop();

    if (err_state()) {
        return -1;
    } else {
        return begin;
    }
}

VAL_LOC_T eval(struct AstNode *node, struct Runtime *rt, struct SymMap *sym_map)
{
    VAL_LOC_T begin, result, end;

    err_reset();
	eval_location_reset();

    begin = rt->stack->top;
    result = eval_impl(node, rt, sym_map);
    end = rt->stack->top;

    if (!err_state()) {
        return result;
    } else {
        stack_collapse(rt->stack, begin, end);
        return -1;
    }
}

