/* Copyright (C) 2014 Krzysztof Stachowiak */

#ifndef VALUE_H
#define VALUE_H

#include <stddef.h>
#include <stdint.h>

#include "ast.h"

#define VAL_SIZE_BYTES 4

#define VAL_HEAD_TYPE_BYTES 4
#define VAL_HEAD_SIZE_BYTES VAL_SIZE_BYTES
#define VAL_HEAD_BYTES (VAL_HEAD_TYPE_BYTES + VAL_HEAD_SIZE_BYTES)

#define VAL_BOOL_BYTES 1
#define VAL_CHAR_BYTES 1
#define VAL_INT_BYTES 8
#define VAL_REAL_BYTES 8

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
    ptrdiff_t location;
};

struct ValueHeader {
    uint32_t type;
    uint32_t size;
};

/* TODO: This is pathological. Change it to a set of structures for each type.
 *       Rely on offsets and header peeking.
 */
struct Value {

    ptrdiff_t begin;
    struct ValueHeader header;

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

    struct {
        struct AstNode *def;
        struct {
            struct Capture *data;
            uint32_t size, cap;
        } captures;
        struct {
            ptrdiff_t *data;
            uint32_t size, cap;
        } applied;
    } function;
};

void val_print(struct Value *value, bool annotate);
void val_free(struct Value *value);

#endif
