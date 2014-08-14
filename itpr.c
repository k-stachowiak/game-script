/* Copyright (C) 2014 Krzysztof Stachowiak */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "itpr.h"

struct Value stack_peek_value(struct Stack *stack, ptrdiff_t location);

static void peek_compound(struct Stack *stack, struct Value *result)
{
	ptrdiff_t location = result->begin + VAL_HEAD_BYTES;
	ptrdiff_t end = result->begin + result->header.size + VAL_HEAD_BYTES;
	struct Value value;

	result->compound.data = NULL;
	result->compound.cap = 0;
	result->compound.size = 0;

	while (location != end) {
		value = stack_peek_value(stack, location);
		ARRAY_APPEND(result->compound, value);
		location += value.header.size + VAL_HEAD_BYTES;
	}
}

static void peek_function(struct Stack *stack, struct Value *result)
{
    ptrdiff_t location = result->begin + VAL_HEAD_BYTES;
    void *impl;
    uint32_t i, size;

    memcpy(&impl, stack->buffer + location, VAL_PTR_BYTES);
    result->function.def = (struct AstNode*)impl;
    location += VAL_PTR_BYTES;

    memcpy(&size, stack->buffer + location, VAL_SIZE_BYTES);
    result->function.captures.data = NULL;
    result->function.captures.size = 0;
    result->function.captures.cap = 0;
    location += VAL_SIZE_BYTES;

    for (i = 0; i < size; ++i) {
        /* Peek captures. */
        exit(3);
    }

    memcpy(&size, stack->buffer + location, VAL_SIZE_BYTES);
    result->function.applied.data = NULL;
    result->function.applied.size = 0;
    result->function.applied.cap = 0;
    location += VAL_SIZE_BYTES;

    for (i = 0; i < size; ++i) {
        /* Peek applied arguments. */
        exit(3);
    }
}

static ptrdiff_t compute_size(struct AstNode *node)
{
    struct AstNode *child;
    ptrdiff_t cpd_len = 0;

    switch (node->type) {
    case AST_COMPOUND:
        child = node->data.compound.exprs;
        while (child) {
            cpd_len += compute_size(child);
            cpd_len += VAL_HEAD_BYTES;
            child = child->next;
        }
        return cpd_len;

    case AST_LITERAL:
        switch (node->data.literal.type) {
        case AST_LIT_BOOL:
            return VAL_BOOL_BYTES;

        case AST_LIT_CHAR:
            return VAL_CHAR_BYTES;

        case AST_LIT_INT:
            return VAL_INT_BYTES;

        case AST_LIT_REAL:
            return VAL_REAL_BYTES;

        case AST_LIT_STRING:
            return strlen(node->data.literal.data.string);

    	default:
    		printf("Unhandled literal type.\n");
    		exit(1);
        }
        break;

	default:
		printf("Unhandled value type.\n");
		exit(1);
    }
}

static void eval_literal(struct AstNode *node, struct Stack *stack)
{
    uint32_t type;
    uint32_t size;
    char *value;

    switch (node->data.literal.type) {
    case AST_LIT_BOOL:
        type = (uint32_t)VAL_BOOL;
        size = VAL_BOOL_BYTES;
        value = (char*)&node->data.literal.data.boolean;
        break;

    case AST_LIT_CHAR:
        type = (uint32_t)VAL_CHAR;
        size = VAL_CHAR_BYTES;
        value = (char*)&node->data.literal.data.character;
        break;

    case AST_LIT_INT:
        type = (uint32_t)VAL_INT;
        size = VAL_INT_BYTES;
        value = (char*)&node->data.literal.data.integer;
        break;

    case AST_LIT_REAL:
        type = (uint32_t)VAL_REAL;
        size = VAL_REAL_BYTES;
        value = (char*)&node->data.literal.data.real;
        break;

    case AST_LIT_STRING:
        value = node->data.literal.data.string;
        type = (uint32_t)VAL_STRING;
        size = strlen(value);
        break;
    }

    stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type);
    stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&size);
    stack_push(stack, size, value);
}

static void eval_func_def(
        struct AstNode *node,
        struct Stack *stack,
        struct SymMap *sym_map)
{
    (void)sym_map;

    static uint32_t type = (uint32_t)VAL_FUNCTION;
    static uint32_t zero = 0;
    auto   uint32_t size = VAL_PTR_BYTES + 2 * VAL_SIZE_BYTES;
    auto   void* impl = (void*)node;

    stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type);
    stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&size);
    stack_push(stack, VAL_PTR_BYTES, (char*)&impl);
    stack_push(stack, VAL_SIZE_BYTES, (char*)&zero);
    stack_push(stack, VAL_SIZE_BYTES, (char*)&zero);
}

static void eval_compound(
		struct AstNode *node,
		struct Stack *stack,
		struct SymMap *sym_map)
{
    uint32_t type;
    uint32_t size = compute_size(node); /* TODO:Do we have to precompute this ? */
    struct AstNode *current = node->data.compound.exprs;

    switch (node->data.compound.type) {
        case AST_CPD_ARRAY:
            type = VAL_ARRAY;
            break;

        case AST_CPD_TUPLE:
            type = VAL_TUPLE;
            break;
    }

    stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type);
    stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&size);

    while (current) {
        eval(current, stack, sym_map);
        current = current->next;
    }
}

static void eval_bind(
		struct AstNode *node,
		struct Stack *stack,
		struct SymMap *sym_map)
{
	ptrdiff_t location = eval(node->data.bind.expr, stack, sym_map);
	sym_map_insert(sym_map, node->data.bind.symbol, location);
}

static void eval_reference(
		struct AstNode *node,
		struct Stack *stack,
		struct SymMap *sym_map)
{
	struct SymMapKvp *kvp = sym_map_find(sym_map, node->data.reference.symbol);
	struct ValueHeader header;
	uint32_t size;

	if (!kvp) {
		/* TODO: Implement unified runtime error handling (SYMBOL NOT FOUND). */
		printf("Symbol \"%s\" not found.\n", node->data.reference.symbol);
		exit(1);
	}

	header = stack_peek_header(stack, kvp->location);
	size = header.size + VAL_HEAD_BYTES;
	stack_push(stack, size, stack->buffer + kvp->location);
}

static void val_print_function(struct Value *value)
{
    struct AstNode *def = value->function.def;
    int arity, captures_count, applied_args_count;

    switch(def->type) {
    case AST_BIF:
        printf("built-in function");
        break;

    case AST_FUNC_DEF:
        captures_count = value->function.captures.size;
        applied_args_count = value->function.applied.size;
        arity = def->data.func_def.func.arg_count - applied_args_count;
        printf(
            "function; arity = %d, captures count = %d, applied args count %d",
            arity, captures_count, applied_args_count);
        break;

    default:
        printf("Attempting to print non-function value with function printing.\n");
        exit(1);
    }
}

void val_print(struct Value *value, bool annotate)
{
    int i;
    char *string;
    ptrdiff_t str_len;

    switch ((enum ValueType)value->header.type) {
    case VAL_BOOL:
        if (annotate) {
            printf("bool :: ");
        }
        printf("%s", value->primitive.boolean ? "true" : "false");
        break;

    case VAL_CHAR:
        if (annotate) {
            printf("char :: ");
        }
        printf("'%c'", value->primitive.character);
        break;

    case VAL_INT:
        if (annotate) {
            printf("integer :: ");
        }
        printf("%ld", value->primitive.integer);
        break;

    case VAL_REAL:
        if (annotate) {
            printf("real :: ");
        }
        printf("%f", value->primitive.real);
        break;

    case VAL_STRING:
        if (annotate) {
            printf("string :: ");
        }
        str_len = value->string.str_len;
        string = malloc(str_len + 1);
        memcpy(string, value->string.str_begin, str_len);
        string[str_len] = '\0';
        printf("%s", string);
        free(string);
        break;

    case VAL_ARRAY:
        if (annotate) {
            printf("array :: ");
        }
        printf("[ ");
        for (i = 0; i < value->compound.size; ++i) {
            val_print(value->compound.data + i, false);
            printf(" ");
        }
        printf("]");
        break;

    case VAL_TUPLE:
        if (annotate) {
            printf("tuple :: ");
        }
        printf("[ ");
        for (i = 0; i < value->compound.size; ++i) {
            val_print(value->compound.data + i, true);
            printf(" ");
        }
        printf("]");
        break;

    case VAL_FUNCTION:
        val_print_function(value);
        break;
    }
}

struct Stack *stack_make(ptrdiff_t size)
{
    struct Stack *result = malloc(sizeof(*result));
    result->buffer = malloc(size);
    result->size = size;
    result->top = 0;
    return result;
}

void stack_free(struct Stack *stack)
{
    free(stack->buffer);
    free(stack);
}

void stack_push(struct Stack *stack, ptrdiff_t size, char *data)
{
    char *dst;

    if (stack->top + size >= stack->size) {
    	/* TODO: Implement unified runtime error handling (OVERFLOW). */
		printf("Stack overflow.\n");
		exit(1);
    }

    dst = stack->buffer + stack->top;
    memcpy(dst, data, size);
    stack->top += size;
}

struct ValueHeader stack_peek_header(struct Stack *stack, ptrdiff_t location)
{
	struct ValueHeader result;
	char *src = stack->buffer + location;
	memcpy(&result.type, src, VAL_HEAD_TYPE_BYTES);
	memcpy(&result.size, src + VAL_HEAD_TYPE_BYTES, VAL_HEAD_SIZE_BYTES);
	return result;
}

struct Value stack_peek_value(struct Stack *stack, ptrdiff_t location)
{
    char *src = stack->buffer + location;
    struct Value result;

    /* TODO: Consider completely untangling value from the stack. */
    result.begin = location;
    result.header = stack_peek_header(stack, location);

    switch ((enum ValueType)result.header.type) {
    case VAL_BOOL:
        memcpy(
        	&(result.primitive.boolean),
        	src + VAL_HEAD_BYTES,
        	VAL_BOOL_BYTES);
        break;

    case VAL_CHAR:
        memcpy(
        	&(result.primitive.character),
        	src + VAL_HEAD_BYTES,
        	VAL_CHAR_BYTES);
        break;

    case VAL_INT:
        memcpy(
        	&(result.primitive.integer),
        	src + VAL_HEAD_BYTES,
        	VAL_INT_BYTES);
        break;

    case VAL_REAL:
        memcpy(
        	&(result.primitive.real),
        	src + VAL_HEAD_BYTES,
        	VAL_REAL_BYTES);
        break;

    case VAL_STRING:
        /* TODO: perform a copy here. */
        result.string.str_begin = src + VAL_HEAD_BYTES;
        result.string.str_len = result.header.size;
        break;

    case VAL_ARRAY:
    case VAL_TUPLE:
    	peek_compound(stack, &result);
    	break;

    case VAL_FUNCTION:
        peek_function(stack, &result);
        break;

    default:
    	printf("Unhandled value type.\n");
    	exit(1);
    }

    return result;
}

void sym_map_init(struct SymMap *sym_map)
{
	sym_map->map = NULL;
	sym_map->end = NULL;
}

void sym_map_deinit(struct SymMap *sym_map)
{
	struct SymMapKvp *temp, *kvp = sym_map->map;
	while (kvp) {
		free(kvp->key);
		temp = kvp;
		kvp = kvp->next;
		free(temp);
	}
	sym_map->map = NULL;
	sym_map->end = NULL;
}

void sym_map_insert(struct SymMap *sym_map, char *key, ptrdiff_t location)
{
	struct SymMapKvp *kvp;
	int len = strlen(key);
	char *key_copy = malloc(len + 1);
	memcpy(key_copy, key, len + 1);

	kvp = sym_map_find(sym_map, key);
	if (kvp) {
		kvp->location = location;
	}

	kvp = malloc(sizeof(*kvp));

	kvp->key = key_copy;
	kvp->location = location;
	kvp->next = NULL;

	LIST_APPEND(kvp, &sym_map->map, &sym_map->end);
}

struct SymMapKvp *sym_map_find(struct SymMap *sym_map, char *key)
{
	struct SymMapKvp *kvp = sym_map->map;
	while (kvp) {
		if (strcmp(kvp->key, key) == 0) {
			return kvp;
		}
		kvp = kvp->next;
	}

	return NULL;
}

ptrdiff_t eval(
		struct AstNode *node,
		struct Stack *stack,
		struct SymMap *sym_map)
{
    ptrdiff_t begin = stack->top;
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

    case AST_FUNC_CALL:
        eval_func_call(node, stack, sym_map);
        break;

	default:
		printf("Unhandled AST node type.\n");
		exit(1);
    }
    return begin;
}
