/* Copyright (C) 2014 Krzysztof Stachowiak */

#ifndef STACK_H
#define STACK_H

#include <stddef.h>
#include <stdbool.h>

#include "define.h"

struct Stack {
    char *buffer;
	VAL_LOC_T size;
	VAL_LOC_T top;
};

struct Stack *stack_make(VAL_LOC_T size);
void stack_free(struct Stack *stack);

VAL_LOC_T stack_push(struct Stack *stack, VAL_LOC_T size, char *data);
void stack_push_copy(struct Stack *stack, VAL_LOC_T location);

VAL_SIZE_T stack_peek_size(struct Stack *stack, VAL_LOC_T loc);
void *stack_peek_ptr(struct Stack *stack, VAL_LOC_T loc);

void stack_collapse(struct Stack *stack, VAL_LOC_T begin, VAL_LOC_T end);
void stack_for_each(struct Stack *stack, void *state, void(*f)(void*, VAL_LOC_T));

#endif
