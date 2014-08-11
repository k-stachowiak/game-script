#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "itpr.h"

void val_print(struct Value *value)
{
    char *string;
    ptrdiff_t str_len;

    switch ((enum ValueType)value->header.type) {
    case VAL_BOOL:
        printf("bool :: %s", value->primitive.boolean ? "true" : "false");
        break;

    case VAL_CHAR:
        printf("char :: '%c'", value->primitive.character);
        break;

    case VAL_INT:
        printf("integer :: %lld", value->primitive.integer);
        break;

    case VAL_REAL:
        printf("real :: %f", value->primitive.real);
        break;

    case VAL_STRING:
        str_len = value->string.str_len;
        string = malloc(str_len + 1);
        memcpy(string, value->string.str_begin, str_len);
        string[str_len] = '\0';
        printf("string :: %s", string);
        free(string);
        break;

    case VAL_ARRAY:
        printf("array");
        break;

    case VAL_TUPLE:
        printf("tuple");
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

struct Value stack_peek(struct Stack *stack, ptrdiff_t location)
{
    uint32_t type, size;
    char *src = stack->buffer + location;
    struct Value result;

    memcpy(&type, src + 0, 4);
    memcpy(&size, src + 4, 4);

    result.begin = location;
    result.header.type = type;
    result.header.size = size;

    switch ((enum ValueType)type) {
    case VAL_BOOL:
        memcpy(&(result.primitive.boolean), src + 8, 1);
        break;
    case VAL_CHAR:
        memcpy(&(result.primitive.character), src + 8, 1);
        break;
    case VAL_INT:
        memcpy(&(result.primitive.integer), src + 8, 8);
        break;
    case VAL_REAL:
        memcpy(&(result.primitive.real), src + 8, 8);
        break;
    case VAL_STRING:
        result.string.str_begin = src + 8;
        result.string.str_len = size;
        break;
    default:
    	printf("Unhandled value type.\n");
    	exit(1);
    }

    return result;
}

static void stack_push(ptrdiff_t size, struct Stack *stack, char *data)
{
    /* TODO: Check for the stack overflow here. */
    char *dst = stack->buffer + stack->top;
    memcpy(dst, data, size);
    stack->top += size;
}

static void stack_push_bool(struct Stack *stack, char *value)
{
    static uint32_t bool_type = (uint32_t)VAL_BOOL;
    static uint32_t bool_size = 1;
    stack_push(4, stack, (char*)&bool_type);
    stack_push(4, stack, (char*)&bool_size);
    stack_push(1, stack, value);
}

static void stack_push_char(struct Stack *stack, char *value)
{
    static uint32_t char_type = (uint32_t)VAL_CHAR;
    static uint32_t char_size = 1;
    stack_push(4, stack, (char*)&char_type);
    stack_push(4, stack, (char*)&char_size);
    stack_push(1, stack, value);
}

static void stack_push_int(struct Stack *stack, char *value)
{
    static uint32_t int_type = (uint32_t)VAL_INT;
    static uint32_t int_size = 8;
    stack_push(4, stack, (char*)&int_type);
    stack_push(4, stack, (char*)&int_size);
    stack_push(8, stack, value);
}

static void stack_push_real(struct Stack *stack, char *value)
{
    static uint32_t real_type = (uint32_t)VAL_REAL;
    static uint32_t real_size = 8;
    stack_push(4, stack, (char*)&real_type);
    stack_push(4, stack, (char*)&real_size);
    stack_push(8, stack, value);
}

static void stack_push_string(struct Stack *stack, char *value)
{
    static uint32_t string_type = (uint32_t)VAL_STRING;
    auto   uint32_t len = strlen(value);
    stack_push(4, stack, (char*)&string_type);
    stack_push(4, stack, (char*)&len);
    stack_push(len, stack, value);
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
        return cpd_len;

    case AST_LITERAL:
        switch (node->data.literal.type) {
        case AST_LIT_BOOL:
            /* TODO: Get rid of magic numbers! */
            return 1;

        case AST_LIT_STRING:
            return strlen(node->data.literal.data.string);

        case AST_LIT_CHAR:
            return 1;

        case AST_LIT_INT:
            return 8;

        case AST_LIT_REAL:
            return 8;

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

static void eval_compound(struct AstNode *node, struct Stack *stack)
{
    static uint32_t array_type = (uint32_t)VAL_ARRAY;
    static uint32_t tuple_type = (uint32_t)VAL_TUPLE;
    auto   uint32_t len = compute_size(node);
    struct AstNode *current = node->data.compound.exprs;

    switch (node->data.compound.type) {
        case AST_CPD_ARRAY:
            stack_push(4, stack, (char*)&array_type);
            break;
        case AST_CPD_TUPLE:
            stack_push(4, stack, (char*)&tuple_type);
            break;
    }

    stack_push(4, stack, (char*)&len);

    while (current) {
        eval(current, stack);
        current = current->next;
    }
}

ptrdiff_t eval(struct AstNode *node, struct Stack *stack)
{
    ptrdiff_t begin = stack->top;
    switch (node->type) {
    case AST_LITERAL:
        eval_literal(node, stack);
        break;
    case AST_COMPOUND:
        eval_compound(node, stack);
        break;
	default:
		printf("Unhandled AST node type.\n");
		exit(1);
    }
    return begin;
}

