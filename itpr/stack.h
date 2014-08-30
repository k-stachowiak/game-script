/* Copyright (C) 2014 Krzysztof Stachowiak */

#include <stddef.h>
#include <stdbool.h>

#ifndef STACK_H
#define STACK_H

struct Stack {
    char *buffer;
    ptrdiff_t size;
    ptrdiff_t top;
};

struct Stack *stack_make(ptrdiff_t size);
void stack_free(struct Stack *stack);
bool stack_push(struct Stack *stack, ptrdiff_t size, char *data);
void stack_collapse(struct Stack *stack, ptrdiff_t begin, ptrdiff_t end);
struct ValueHeader stack_peek_header(struct Stack *stack, ptrdiff_t location);
struct Value stack_peek_value(struct Stack *stack, ptrdiff_t location);

#endif
