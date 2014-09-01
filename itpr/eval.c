/* Copyright (C) 2014 Krzysztof Stachowiak */

#include <string.h>
#include <stdlib.h>

#include "eval.h"
#include "eval_detail.h"
#include "value.h"
#include "bif.h"
#include "error.h"

static ptrdiff_t eval_impl(
        struct AstNode *node,
        struct Stack *stack,
        struct SymMap *sym_map);

/* Function call evaluation.
 * =========================
 */

static bool eval_func_call_lookup_value(
		char *symbol,
		struct SymMap *sym_map,
        struct Stack *stack,
		struct Value *result)
{
	struct SymMapKvp *kvp;

	if (!(kvp = sym_map_find(sym_map, symbol))) {
        err_set(ERR_EVAL, "Requested function doesn't exist.");
        return false;
    }

    *result = stack_peek_value(stack, kvp->location);
    if (result->header.type != (VAL_HEAD_TYPE_T)VAL_FUNCTION) {
        err_set(ERR_EVAL, "Requested call to a non-function value.");
		return false;
    }

    return true;
}

static int eval_func_call_compute_arity(struct AstNode *def_node)
{
    switch (def_node->type) {
    case AST_BIF:
        switch (def_node->data.bif.type) {
        case AST_BIF_ARYTHM_UNARY:
            return 1;
		case AST_BIF_ARYTHM_BINARY:
		case AST_BIF_COMPARE:
            return 2;
        default:
            printf("Unhandled bif type.\n");
            exit(1);
        }
        break;

    case AST_FUNC_DEF:
        return def_node->data.func_def.func.arg_count;
        break;

    default:
        printf("Non-function AST node pointed by function value.\n");
        exit(1);
    }
}

static void eval_func_call_curry_on(
        struct AstNode *call_node,
        struct Stack *stack,
        struct SymMap *sym_map,
        struct Value *value)
{
    static VAL_HEAD_TYPE_T type = (VAL_HEAD_TYPE_T)VAL_FUNCTION;
    static VAL_HEAD_SIZE_T zero = 0;
    struct AstNode *arg_node;
    VAL_SIZE_T applied_count = 0;
    VAL_LOC_T size_loc, appl_count_loc, data_begin, data_size;
    VAL_SIZE_T i;

    /* Header. */
    stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type);
    size_loc = stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&zero);    

    /* Pointer to implementation. */
	data_begin = stack_push(stack, VAL_PTR_BYTES, (char*)&value->function.def);

    /* Captures. */
    stack_push(stack, VAL_SIZE_BYTES, (char*)&value->function.captures.size);
    for (i = 0; i < value->function.captures.size; ++i) {
        struct Capture *cap = value->function.captures.data + i;
        VAL_SIZE_T len = strlen(cap->symbol);
        struct ValueHeader header = stack_peek_header(stack, cap->location);
        stack_push(stack, VAL_SIZE_BYTES, (char*)&len);
        stack_push(stack, len, cap->symbol);
        stack_push(stack, header.size + VAL_HEAD_BYTES, stack->buffer + cap->location);
    }

    /* Already applied. */
    appl_count_loc = stack_push(stack, VAL_SIZE_BYTES, (char*)&zero);
    for (i = 0; i < value->function.applied.size; ++i) {
        VAL_LOC_T applied_loc = value->function.applied.data[i];
        struct ValueHeader header = stack_peek_header(stack, applied_loc);
        stack_push(stack, header.size + VAL_HEAD_BYTES, stack->buffer + applied_loc);
        ++applied_count;
    }

    /* New applied. */
    arg_node = call_node->data.func_call.actual_args;
    while (arg_node) {
        eval_impl(arg_node, stack, sym_map);
        if (err_state()) {
            return;
        }
        arg_node = arg_node->next;
        ++applied_count;
    }

    /* Hack value size to correct value. */
    data_size = stack->top - data_begin;
    memcpy(stack->buffer + size_loc, &data_size, VAL_SIZE_BYTES);

    /* Hack applied count to correct value. */
    memcpy(stack->buffer + appl_count_loc, &applied_count, VAL_SIZE_BYTES);
}

static void eval_func_call_final_bif(
        struct AstNode *arg_list,
        struct Stack *stack,
        struct SymMap *sym_map,
        struct Value *value)
{
    static VAL_HEAD_TYPE_T type_int = (VAL_HEAD_TYPE_T)VAL_INT;
    static VAL_HEAD_SIZE_T size_int = VAL_INT_BYTES;
    static VAL_HEAD_TYPE_T type_real = (VAL_HEAD_TYPE_T)VAL_REAL;
    static VAL_HEAD_SIZE_T size_real = VAL_REAL_BYTES;
	static VAL_HEAD_TYPE_T type_bool = (VAL_HEAD_TYPE_T)VAL_BOOL;
	static VAL_HEAD_SIZE_T size_bool = VAL_BOOL_BYTES;
    struct AstBif *impl = &value->function.def->data.bif;
    struct Value args[BIF_MAX_ARITY];
    VAL_SIZE_T arg_count = 0;
    VAL_LOC_T temp_begin, temp_end;
    struct AstNode *arg_node;
    VAL_SIZE_T i;

    /* Peal out already applied args. */
    for (i = 0; i < value->function.applied.size; ++i) {
        VAL_LOC_T loc = value->function.applied.data[i];
        args[arg_count++] = stack_peek_value(stack, loc);
    }

    /* Evaluate the missing args. */
    temp_begin = stack->top;
    arg_node = arg_list;
    while (arg_node) {
		if (arg_count == BIF_MAX_ARITY) {
			printf("BIF called with incorrect arguments count.\n");
			exit(1);
		}
        VAL_LOC_T loc = stack->top;
        eval_impl(arg_node, stack, sym_map);
        if (err_state()) {
            return;
        }
        args[arg_count++] = stack_peek_value(stack, loc);
        arg_node = arg_node->next;
    }
    temp_end = stack->top;

    /* Evaluate the function implementation. */
    switch (impl->type) {
    case AST_BIF_ARYTHM_UNARY:
        if ((enum ValueType)args[0].header.type == VAL_INT) {
            VAL_INT_T result = impl->un_int_impl(args[0].primitive.integer);
            stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type_int);
            stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&size_int);
            stack_push(stack, size_int, (char*)&result);

        } else if ((enum ValueType)args[0].header.type == VAL_REAL) {
            VAL_REAL_T result = impl->un_real_impl(args[0].primitive.real);
            stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type_real);
            stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&size_real);
            stack_push(stack, size_real, (char*)&result);

        } else {
            err_set(ERR_EVAL, "Non-arythmetic type passed to arithmetic BIF.");
            return;
        }
        break;

    case AST_BIF_ARYTHM_BINARY:
        if ((enum ValueType)args[0].header.type == VAL_INT &&
            (enum ValueType)args[1].header.type == VAL_INT) {
			VAL_INT_T result = impl->bin_int_impl(
                        args[0].primitive.integer,
                        args[1].primitive.integer);
                stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type_int);
                stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&size_int);
                stack_push(stack, size_int, (char*)&result);

        } else if ((enum ValueType)args[0].header.type == VAL_REAL &&
                   (enum ValueType)args[1].header.type == VAL_REAL) {
			VAL_REAL_T result = impl->bin_real_impl(
                    args[0].primitive.real,
                    args[1].primitive.real);
            stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type_real);
            stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&size_real);
            stack_push(stack, size_real, (char*)&result);

        } else {
            err_set(ERR_EVAL, "Non-arythmetic type passed to arithmetic BIF.");
            return;
        }
        break;

	case AST_BIF_COMPARE:
		if ((enum ValueType)args[0].header.type == VAL_INT &&
		    (enum ValueType)args[1].header.type == VAL_INT) {
			VAL_BOOL_T result = impl->cmp_int_impl(
				args[0].primitive.integer,
				args[1].primitive.integer);
			stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type_bool);
			stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&size_bool);
			stack_push(stack, size_bool, (char*)&result);

}
		else if ((enum ValueType)args[0].header.type == VAL_REAL &&
			     (enum ValueType)args[1].header.type == VAL_REAL) {
			VAL_BOOL_T result = impl->cmp_real_impl(
				args[0].primitive.real,
				args[1].primitive.real);
			stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type_bool);
			stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&size_bool);
			stack_push(stack, size_bool, (char*)&result);

		}
		else {
			err_set(ERR_EVAL, "Non-arythmetic type passed to arithmetic BIF.");
			return;
		}
    }

    /* Collapse the temporaries. */
    stack_collapse(stack, temp_begin, temp_end);
}

static void eval_func_call_final_def(
        struct AstNode *arg_list,
        struct Stack *stack,
        struct SymMap *sym_map,
        struct Value *value)
{
    VAL_SIZE_T i;
    VAL_LOC_T temp_begin, temp_end;
    struct SymMap local_sym_map;
    struct AstNode *arg_node, *exe_node, *impl;
    struct AstCommonFunc *func;
    char **formal_arg;

    /* Create the local scope. */
    sym_map_init(&local_sym_map, sym_map, stack);

    /* Insert captures into the scope. */
    for (i = 0; i < value->function.captures.size; ++i) {
        struct Capture *cap = value->function.captures.data + i;
        sym_map_insert(&local_sym_map, cap->symbol, cap->location);
    }

    /* Begin inserting arguments into the scope. */
    impl = value->function.def;
    func = &impl->data.func_def.func;
    formal_arg = func->formal_args;

    /* Insert already applied arguments. */
    for (i = 0; i < value->function.applied.size; ++i) {
        VAL_LOC_T loc = value->function.applied.data[i];
        sym_map_insert(&local_sym_map, *formal_arg, loc);
        ++formal_arg;
    }

    /* Insert new arguments. */
    temp_begin = stack->top;
    arg_node = arg_list;
    while (arg_node) {
        VAL_LOC_T loc = stack->top;
        eval_impl(arg_node, stack, sym_map);
        if (err_state()) {
            return;
        }
        sym_map_insert(&local_sym_map, *formal_arg, loc);
        ++formal_arg;
        arg_node = arg_node->next;
    }
    temp_end = stack->top;

    /* Evaluate the function expression. */
    exe_node = impl->data.func_def.exprs;
    while (exe_node) {
        eval_impl(exe_node, stack, &local_sym_map);
        if (err_state()) {
            return;
        }
        exe_node = exe_node->next;
    }

    /* Collapse the temporaries. */
    stack_collapse(stack, temp_begin, temp_end);
}

static void eval_func_call(
        struct AstNode *call_node,
        struct Stack *stack,
        struct SymMap *sym_map)
{
    struct Value value;
    VAL_SIZE_T arity, applied_args, new_args;

	if (!eval_func_call_lookup_value(
			call_node->data.func_call.symbol,
			sym_map, stack, &value)) {
        return;
    }

    arity = eval_func_call_compute_arity(value.function.def);
    applied_args = value.function.applied.size;
    new_args = ast_list_len(call_node->data.func_call.actual_args);

    if (applied_args + new_args < arity) {
        eval_func_call_curry_on(call_node, stack, sym_map, &value);

    } else if (applied_args + new_args == arity) {
        switch (value.function.def->type) {
        case AST_BIF:
            eval_func_call_final_bif(
                call_node->data.func_call.actual_args, stack, sym_map, &value);
            break;

        case AST_FUNC_DEF:
            eval_func_call_final_def(
                call_node->data.func_call.actual_args, stack, sym_map, &value);
            break;

        default:
            printf("Non-function AST node pointed by function value.\n");
            exit(1);
        }

    } else {
        err_set(ERR_EVAL, "Too many arguments provided to a function call.");
    }
}

/* Simple evaluation functions.
 * ============================
 */

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

static VAL_LOC_T eval_impl(
		struct AstNode *node,
		struct Stack *stack,
		struct SymMap *sym_map)
{
    VAL_LOC_T begin = stack->top;

    switch (node->type) {
    case AST_LITERAL:
        eval_literal(node, stack);
        break;

    case AST_COMPOUND:
        eval_compound(node, stack, sym_map);
        break;

    case AST_BIND:
    	eval_bind(node, stack, sym_map);
    	break;

    case AST_REFERENCE:
    	eval_reference(node, stack, sym_map);
    	break;

    case AST_FUNC_DEF:
        eval_func_def(node, stack, sym_map);
        break;

    case AST_BIF:
        eval_bif(node, stack);
        break;

    case AST_FUNC_CALL:
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
