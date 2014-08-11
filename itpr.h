#ifndef ITPR_H
#define ITPR_H

#include <stdint.h>
#include <stddef.h>

#include "ast.h"

#define VAL_HEAD_TYPE_BYTES 4
#define VAL_HEAD_SIZE_BYTES 4
#define VAL_HEAD_BYTES (VAL_HEAD_TYPE_BYTES + VAL_HEAD_SIZE_BYTES)

#define VAL_BOOL_BYTES 1
#define VAL_CHAR_BYTES 1
#define VAL_INT_BYTES 8
#define VAL_REAL_BYTES 8

enum ValueType {
    VAL_BOOL,
    VAL_CHAR,
    VAL_INT,
    VAL_REAL,
    VAL_STRING,
    VAL_ARRAY,
    VAL_TUPLE
};

struct Value {

    ptrdiff_t begin;

    struct {
        uint32_t type;
        uint32_t size;
    } header;

    union {
        uint8_t boolean;
        uint8_t character;
        int64_t integer;
        double real;
    } primitive;

    struct {
    	struct Value *data;
    	int size, cap;
    } compound;

    struct {
        char *str_begin;
        ptrdiff_t str_len;
    } string;
};

void val_print(struct Value *value, bool annotate);

struct Stack {
    char *buffer;
    ptrdiff_t size;
    ptrdiff_t top;
};

struct Stack *stack_make(ptrdiff_t size);
void stack_free(struct Stack *stack);
struct Value stack_peek(struct Stack *stack, ptrdiff_t location);

ptrdiff_t eval(struct AstNode *node, struct Stack *stack);

#endif
