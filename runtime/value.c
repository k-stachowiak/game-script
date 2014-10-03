/* Copyright (C) 2014 Krzysztof Stachowiak */

#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "value.h"
#include "stack.h"

static void val_print_function(struct Value *value)
{
    struct AstNode *def = value->function.def;
    VAL_SIZE_T arity, captures_count, applied_args_count;

    switch(def->type) {
    case AST_BIF:
        printf("built-in function");
        break;

    case AST_FUNC_DEF:
        captures_count = value->function.captures.size;
        applied_args_count = value->function.applied.size;
        arity = def->data.func_def.func.arg_count - applied_args_count;
        printf(
            "function (ar=%d, cap=%d, appl=%d)",
            arity, captures_count, applied_args_count);
        break;

    default:
		LOG_ERROR("Attempting to print non-function value with function printing.\n");
        exit(1);
    }
}

void val_print(struct Stack* stack, VAL_LOC_T loc, bool annotate)
{
    VAL_SIZE_T i;
    VAL_LOC_T cpd_loc;
    int cpd_size;
    struct ValueHeader cpd_header;
    struct Value value = stack_peek_value(stack, loc);

    switch (val_type(stack, loc)) {
    case VAL_BOOL:
        if (annotate) {
            printf("bool :: ");
        }
        printf("%s", val_bool(stack, loc) ? "true" : "false");
        break;

    case VAL_CHAR:
        if (annotate) {
            printf("char :: ");
        }
        printf("'%c'", val_char(stack, loc));
        break;

    case VAL_INT:
        if (annotate) {
            printf("integer :: ");
        }
        printf("%" PRId64 , val_int(stack, loc));
        break;

    case VAL_REAL:
        if (annotate) {
            printf("real :: ");
        }
        printf("%f", val_real(stack, loc));
        break;

    case VAL_STRING:
        if (annotate) {
            printf("string :: ");
        }
        printf("%s", val_string(stack, loc));
        break;

    case VAL_ARRAY:
        if (annotate) {
            printf("array :: ");
        }
        printf("[ ");
        cpd_size = val_cpd_size(stack, loc);
        cpd_loc = val_cpd_first(stack, loc);
        for (i = 0; i < cpd_size; ++i) {
            val_print(stack, cpd_loc, false);
            printf(" ");
            cpd_loc = val_cpd_next(stack, cpd_loc);
        }
        printf("]");
        break;

    case VAL_TUPLE:
        if (annotate) {
            printf("tuple :: ");
        }
        printf("{ ");
        cpd_size = val_cpd_size(stack, loc);
        cpd_loc = val_cpd_first(stack, loc);
        for (i = 0; i < value.compound.size; ++i) {
            val_print(stack, cpd_loc, false);
            printf(" ");
            cpd_loc = val_cpd_next(stack, cpd_loc);
        }
        printf("}");
        break;

    case VAL_FUNCTION:
        val_print_function(&value);
        break;
    }
}

bool val_eq_int(struct Value *value, VAL_INT_T x)
{
	if ((enum ValueType)value->header.type != VAL_INT) {
		return false;
	}

	return value->primitive.integer == x;
}

enum ValueType val_type(struct Stack *stack, VAL_LOC_T loc)
{
    struct ValueHeader header = stack_peek_header(stack, loc);
    return (enum ValueType)header.type;
}

VAL_BOOL_T val_bool(struct Stack *stack, VAL_LOC_T loc)
{
    VAL_BOOL_T result;
    memcpy(&result, stack->buffer + loc + VAL_HEAD_BYTES, VAL_BOOL_BYTES);
    return result;
}

VAL_CHAR_T val_char(struct Stack *stack, VAL_LOC_T loc)
{
    VAL_CHAR_T result;
    memcpy(&result, stack->buffer + loc + VAL_HEAD_BYTES, VAL_CHAR_BYTES);
    return result;
}

VAL_INT_T val_int(struct Stack *stack, VAL_LOC_T loc)
{
    VAL_INT_T result;
    memcpy(&result, stack->buffer + loc + VAL_HEAD_BYTES, VAL_INT_BYTES);
    return result;
}

VAL_REAL_T val_real(struct Stack *stack, VAL_LOC_T loc)
{
    VAL_REAL_T result;
    memcpy(&result, stack->buffer + loc + VAL_HEAD_BYTES, VAL_REAL_BYTES);
    return result;
}

char* val_string(struct Stack *stack, VAL_LOC_T loc)
{
    return stack->buffer + loc + VAL_HEAD_BYTES;
}

int val_cpd_size(struct Stack *stack, VAL_LOC_T loc)
{
    struct ValueHeader header = stack_peek_header(stack, loc);
    VAL_LOC_T current = val_cpd_first(stack, loc), end = current + header.size;
    int result = 0;
    while (current != end) {
        header = stack_peek_header(stack, current);
        current += VAL_HEAD_BYTES + header.size;
        ++result;
    }
    return result;
}

VAL_LOC_T val_cpd_first(struct Stack *stack, VAL_LOC_T loc)
{
    return loc + VAL_HEAD_BYTES;
}

VAL_LOC_T val_cpd_next(struct Stack *stack, VAL_LOC_T loc)
{
    struct ValueHeader header = stack_peek_header(stack, loc);
    return loc + VAL_HEAD_BYTES + header.size;
}

