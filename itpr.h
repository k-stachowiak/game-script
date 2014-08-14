/* Copyright (C) 2014 Krzysztof Stachowiak */

#ifndef ITPR_H
#define ITPR_H

#include <stdint.h>
#include <stddef.h>

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

struct Capture;

struct ValueHeader {
	uint32_t type;
	uint32_t size;
};

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

struct Stack {
    char *buffer;
    ptrdiff_t size;
    ptrdiff_t top;
};

struct Stack *stack_make(ptrdiff_t size);
void stack_free(struct Stack *stack);
void stack_push(struct Stack *stack, ptrdiff_t size, char *data);
struct ValueHeader stack_peek_header(struct Stack *stack, ptrdiff_t location);
struct Value stack_peek_value(struct Stack *stack, ptrdiff_t location);

struct SymMap {
	struct SymMapKvp *map;
	struct SymMapKvp *end;
};

struct SymMapKvp {
	char *key;
	ptrdiff_t location;
	struct SymMapKvp *next;
};

void sym_map_init(struct SymMap *sym_map);
void sym_map_deinit(struct SymMap *sym_map);
void sym_map_insert(struct SymMap *sym_map, char *key, ptrdiff_t location);
struct SymMapKvp *sym_map_find(struct SymMap *sym_map, char *key);

struct Capture {
	char *symbol;
    struct Location loc;
    ptrdiff_t location;
};

ptrdiff_t eval(
		struct AstNode *node,
		struct Stack *stack,
		struct SymMap *sym_map);

#endif
