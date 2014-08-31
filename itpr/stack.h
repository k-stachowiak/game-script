/* Copyright (C) 2014 Krzysztof Stachowiak */

#ifndef STACK_H
#define STACK_H

#include <stddef.h>
#include <stdbool.h>

#include "common.h"

struct Stack {
    char *buffer;
	VAL_LOC_T size;
	VAL_LOC_T top;
};

struct Stack *stack_make(VAL_LOC_T size);
void stack_free(struct Stack *stack);
VAL_LOC_T stack_push(struct Stack *stack, VAL_LOC_T size, char *data);
void stack_collapse(struct Stack *stack, VAL_LOC_T begin, VAL_LOC_T end);
struct ValueHeader stack_peek_header(struct Stack *stack, VAL_LOC_T location);
struct Value stack_peek_value(struct Stack *stack, VAL_LOC_T location);

#endif
