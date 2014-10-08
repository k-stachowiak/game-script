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

/* Basic pushing API. */

VAL_LOC_T stack_push(struct Stack *stack, VAL_LOC_T size, char *data);
void stack_push_bool(struct Stack *stack, VAL_BOOL_T value);
void stack_push_char(struct Stack *stack, VAL_CHAR_T value);
void stack_push_int(struct Stack *stack, VAL_INT_T value);
void stack_push_real(struct Stack *stack, VAL_REAL_T value);
void stack_push_string(struct Stack *stack, char *value);
void stack_push_copy(struct Stack *stack, VAL_LOC_T location);

/* Composite pushing API. */

void stack_push_array_init(struct Stack *stack, VAL_LOC_T *size_loc);
void stack_push_tuple_init(struct Stack *stack, VAL_LOC_T *size_loc);
void stack_push_cpd_final(struct Stack *stack, VAL_LOC_T size_loc, VAL_SIZE_T size);

void stack_push_func_init(struct Stack *stack, VAL_LOC_T *size_loc, VAL_LOC_T *data_begin, void *func_def);
void stack_push_func_cap_init_deferred(struct Stack *stack, VAL_LOC_T *cap_count_loc);
void stack_push_func_cap_init(struct Stack *stack, VAL_SIZE_T cap_count);
void stack_push_func_cap(struct Stack *stack, char *symbol, VAL_LOC_T loc);
void stack_push_func_cap_copy(struct Stack *stack, VAL_LOC_T loc);
void stack_push_func_cap_final_deferred(struct Stack *stack, VAL_LOC_T cap_count_loc, VAL_SIZE_T cap_count);
void stack_push_func_appl_init(struct Stack *stack, VAL_SIZE_T appl_count);
void stack_push_func_appl_empty(struct Stack *stack);
void stack_push_func_final(struct Stack *stack, VAL_LOC_T size_loc, VAL_SIZE_T data_begin);

/* Others. */

void stack_collapse(struct Stack *stack, VAL_LOC_T begin, VAL_LOC_T end);
struct ValueHeader stack_peek_header(struct Stack *stack, VAL_LOC_T location);
void stack_for_each(struct Stack *stack, void *state, void(*f)(void*, VAL_LOC_T));

#endif
