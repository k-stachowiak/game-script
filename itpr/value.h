/* Copyright (C) 2014 Krzysztof Stachowiak */

#ifndef VALUE_H
#define VALUE_H

#include <stddef.h>
#include <stdint.h>

#include "ast.h"

#define VAL_LOC_T ptrdiff_t

#define VAL_SIZE_T uint32_t
#define VAL_SIZE_BYTES sizeof(VAL_SIZE_T)

#define VAL_HEAD_TYPE_T uint32_t
#define VAL_HEAD_SIZE_T VAL_SIZE_T
#define VAL_HEAD_TYPE_BYTES sizeof(VAL_HEAD_TYPE_T)
#define VAL_HEAD_SIZE_BYTES VAL_SIZE_BYTES
#define VAL_HEAD_BYTES (VAL_HEAD_TYPE_BYTES + VAL_HEAD_SIZE_BYTES)

#define VAL_BOOL_T uint8_t
#define VAL_CHAR_T uint8_t
#define VAL_INT_T uint64_t
#define VAL_REAL_T double

#define VAL_BOOL_BYTES sizeof(VAL_BOOL_T)
#define VAL_CHAR_BYTES sizeof(VAL_CHAR_T)
#define VAL_INT_BYTES sizeof(VAL_INT_T)
#define VAL_REAL_BYTES sizeof(VAL_REAL_T)

#define VAL_PTR_BYTES sizeof(void*)

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

struct Capture {
    char *symbol;
    VAL_LOC_T location;
};

struct ValueHeader {
    VAL_HEAD_TYPE_T type;
    VAL_HEAD_SIZE_T size;
};

union ValuePrimitive {
    VAL_BOOL_T boolean;
    VAL_CHAR_T character;
    VAL_INT_T integer;
    VAL_REAL_T real;
};

struct ValueCompound {
    struct Value *data;
    VAL_SIZE_T size, cap;
};

struct ValueString {
    char *str_begin;
    VAL_LOC_T str_len;
};

struct ValueFunction {
    struct AstNode *def;
    struct {
        struct Capture *data;
        VAL_SIZE_T size, cap;
    } captures;
    struct {
        VAL_LOC_T *data;
        VAL_SIZE_T size, cap;
    } applied;
};

struct Value {
    struct ValueHeader header;
    union ValuePrimitive primitive;
    struct ValueCompound compound;
    struct ValueString string;
    struct ValueFunction function;
};

void val_print(struct Value *value, bool annotate);
void val_free(struct Value *value);

#endif
