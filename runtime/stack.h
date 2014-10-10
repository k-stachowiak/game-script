/* Copyright (C) 2014 Krzysztof Stachowiak */

#ifndef STACK_H
#define STACK_H

#include <stddef.h>
#include <stdbool.h>

#include "common.h"

enum ValueType {
    VAL_BOOL,
    VAL_CHAR,
    VAL_INT,
    VAL_REAL,
    VAL_STRING,
    VAL_ARRAY,
    VAL_TUPLE,
    VAL_FUNCTION
};

struct ValueHeader {
    VAL_HEAD_TYPE_T type;
    VAL_HEAD_SIZE_T size;
};

struct Stack {
    char *buffer;
	VAL_LOC_T size;
	VAL_LOC_T top;
};

struct Stack *stack_make(VAL_LOC_T size);
void stack_free(struct Stack *stack);

VAL_LOC_T stack_push(struct Stack *stack, VAL_LOC_T size, char *data);
void stack_push_copy(struct Stack *stack, VAL_LOC_T location);

void stack_collapse(struct Stack *stack, VAL_LOC_T begin, VAL_LOC_T end);
void stack_for_each(struct Stack *stack, void *state, void(*f)(void*, VAL_LOC_T));

struct ValueHeader stack_peek_header(struct Stack *stack, VAL_LOC_T location);

#endif
