/* Copyright (C) 2014 Krzysztof Stachowiak */

#ifndef VALUE_H
#define VALUE_H

#include <stddef.h>
#include <stdint.h>

#include "ast.h"

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
