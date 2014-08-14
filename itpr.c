/* Copyright (C) 2014 Krzysztof Stachowiak */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "itpr.h"

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
    	printf("function");
    	break;
    }
}

struct Value stack_peek_value(struct Stack *stack, ptrdiff_t location);

static void peek_compound(struct Stack *stack, struct Value *result)
{
	ptrdiff_t location = result->begin + VAL_HEAD_BYTES;
	ptrdiff_t end = result->begin + result->header.size;
	struct Value value;

	result->compound.data = NULL;
	result->compound.cap = 0;
	result->compound.size = 0;

	while (location != end) {
		value = stack_peek_value(stack, location);
		ARRAY_APPEND(result->compound, value);
		location += value.header.size;
	}
}

static void stack_push_bool(struct Stack *stack, char *value)
{
    static uint32_t type = (uint32_t)VAL_BOOL;
    static uint32_t size = VAL_HEAD_BYTES + VAL_BOOL_BYTES;
    stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type);
    stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&size);
    stack_push(stack, VAL_BOOL_BYTES, value);
}

static void stack_push_char(struct Stack *stack, char *value)
{
    static uint32_t type = (uint32_t)VAL_CHAR;
    static uint32_t size = VAL_HEAD_BYTES + VAL_CHAR_BYTES;
    stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type);
    stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&size);
    stack_push(stack, VAL_CHAR_BYTES, value);
}

static void stack_push_int(struct Stack *stack, char *value)
{
    static uint32_t type = (uint32_t)VAL_INT;
    static uint32_t size = VAL_HEAD_BYTES + VAL_INT_BYTES;
    stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type);
    stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&size);
    stack_push(stack, VAL_INT_BYTES, value);
}

static void stack_push_real(struct Stack *stack, char *value)
{
    static uint32_t type = (uint32_t)VAL_REAL;
    static uint32_t size = VAL_HEAD_BYTES + VAL_REAL_BYTES;
    stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type);
    stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&size);
    stack_push(stack, VAL_REAL_BYTES, value);
}

static void stack_push_string(struct Stack *stack, char *value)
{
    static uint32_t type = (uint32_t)VAL_STRING;
    auto   uint32_t len = strlen(value);
    auto   uint32_t size = VAL_HEAD_BYTES + len;
    stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type);
    stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&size);
    stack_push(stack, len, value);
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
            child = child->next;
        }
        return VAL_HEAD_BYTES + cpd_len;

    case AST_LITERAL:
        switch (node->data.literal.type) {
        case AST_LIT_BOOL:
            return VAL_HEAD_BYTES + VAL_BOOL_BYTES;

        case AST_LIT_CHAR:
            return VAL_HEAD_BYTES + VAL_CHAR_BYTES;

        case AST_LIT_INT:
            return VAL_HEAD_BYTES + VAL_INT_BYTES;

        case AST_LIT_REAL:
            return VAL_HEAD_BYTES + VAL_REAL_BYTES;

        case AST_LIT_STRING:
            return VAL_HEAD_BYTES + strlen(node->data.literal.data.string);

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
    switch (node->data.literal.type) {
    case AST_LIT_BOOL:
        stack_push_bool(stack, (char*)&(node->data.literal.data.boolean));
        break;
    case AST_LIT_CHAR:
        stack_push_char(stack, (char*)&(node->data.literal.data.character));
        break;
    case AST_LIT_INT:
        stack_push_int(stack, (char*)&(node->data.literal.data.integer));
        break;
    case AST_LIT_REAL:
        stack_push_real(stack, (char*)&(node->data.literal.data.real));
        break;
    case AST_LIT_STRING:
        stack_push_string(stack, (char*)node->data.literal.data.string);
        break;
    }
}

static void eval_compound(
		struct AstNode *node,
		struct Stack *stack,
		struct SymMap *sym_map)
{
    static uint32_t array_type = (uint32_t)VAL_ARRAY;
    static uint32_t tuple_type = (uint32_t)VAL_TUPLE;
    auto   uint32_t size = compute_size(node); /* TODO:Do we have to precompute this ? */
    struct AstNode *current = node->data.compound.exprs;

    switch (node->data.compound.type) {
        case AST_CPD_ARRAY:
            stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&array_type);
            break;
        case AST_CPD_TUPLE:
            stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&tuple_type);
            break;
    }

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

	if (!kvp) {
		/* TODO: Implement unified runtime error handling (SYMBOL NOT FOUND). */
		printf("Symbol \"%s\" not found.\n", node->data.reference.symbol);
		exit(1);
	}

	header = stack_peek_header(stack, kvp->location);
	stack_push(stack, header.size, stack->buffer + kvp->location);
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
        result.string.str_begin = src + VAL_HEAD_BYTES;
        result.string.str_len = result.header.size - VAL_HEAD_BYTES;
        break;
    case VAL_ARRAY:
    case VAL_TUPLE:
    	peek_compound(stack, &result);
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
	default:
		printf("Unhandled AST node type.\n");
		exit(1);
    }
    return begin;
}
