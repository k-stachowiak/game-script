/* Copyright (C) 2014 Krzysztof Stachowiak */

#include <stdlib.h>
#include <string.h>

#include "value.h"

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