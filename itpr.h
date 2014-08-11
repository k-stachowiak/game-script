/* Copyright (C) 2014 Krzysztof Stachowiak */

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
    VAL_TUPLE,
    VAL_FUNCTION
};

struct Capture;

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

    struct {
    	struct AstNode *def;
    	struct {
    		struct Capture *data;
    		int size, cap;
    	} captures;
    	struct {
    		struct Value *data;
    		int size, cap;
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
struct Value stack_peek(struct Stack *stack, ptrdiff_t location);

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
    struct Value value;
};

ptrdiff_t eval(
		struct AstNode *node,
		struct Stack *stack,
		struct SymMap *sym_map);

#endif
