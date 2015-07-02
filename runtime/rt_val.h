/* Copyright (C) 2014 Krzysztof Stachowiak */

#ifndef RT_VAL_H
#define RT_VAL_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define VAL_LOC_T ptrdiff_t

#define VAL_TYPE_T uint8_t
#define VAL_TYPE_BYTES sizeof(VAL_TYPE_T)
#define VAL_SIZE_T uint16_t
#define VAL_SIZE_BYTES sizeof(VAL_SIZE_T)

#define VAL_HEAD_TYPE_T VAL_TYPE_T
#define VAL_HEAD_SIZE_T VAL_SIZE_T
#define VAL_HEAD_TYPE_BYTES VAL_TYPE_BYTES
#define VAL_HEAD_SIZE_BYTES VAL_SIZE_BYTES
#define VAL_HEAD_BYTES (VAL_HEAD_TYPE_BYTES + VAL_HEAD_SIZE_BYTES)

#define VAL_BOOL_T char
#define VAL_CHAR_T char
#define VAL_INT_T int64_t
#define VAL_REAL_T double
#define VAL_REF_T VAL_LOC_T

#define VAL_BOOL_BYTES sizeof(VAL_BOOL_T)
#define VAL_CHAR_BYTES sizeof(VAL_CHAR_T)
#define VAL_INT_BYTES sizeof(VAL_INT_T)
#define VAL_REAL_BYTES sizeof(VAL_REAL_T)
#define VAL_REF_BYTES sizeof(VAL_REF_T)
#define VAL_UNIT_BYTES 0

#define VAL_PTR_BYTES sizeof(void*)

/* Allocate variables of significant values to copy from. */
extern VAL_HEAD_SIZE_T zero;
extern VAL_HEAD_SIZE_T bool_size;
extern VAL_HEAD_SIZE_T char_size;
extern VAL_HEAD_SIZE_T int_size;
extern VAL_HEAD_SIZE_T real_size;
extern VAL_HEAD_SIZE_T ref_size;
extern VAL_HEAD_SIZE_T unit_size;

struct Runtime;
struct Stack;

/* Data structures.
 * ================
 */

enum ValueType {
    VAL_BOOL,
    VAL_CHAR,
    VAL_INT,
    VAL_REAL,
    VAL_ARRAY,
    VAL_TUPLE,
    VAL_FUNCTION,
	VAL_REF,
    VAL_UNIT
};

struct ValueHeader {
    VAL_HEAD_TYPE_T type;
    VAL_HEAD_SIZE_T size;
};

enum ValueFuncType {
	VAL_FUNC_AST,
	VAL_FUNC_BIF,
	VAL_FUNC_CLIF
};

struct ValueFuncData {
    VAL_LOC_T arity_loc;
    VAL_LOC_T type_loc;
    VAL_LOC_T impl_loc;
    VAL_LOC_T cap_start;
    VAL_LOC_T appl_start;

    VAL_SIZE_T arity;
    VAL_SIZE_T appl_count;
    VAL_SIZE_T cap_count;
	VAL_TYPE_T func_type;
	void *impl;
};

/* Writing (pushing) API.
 * ======================
 */

void rt_val_push_copy(struct Stack *stack, VAL_LOC_T location);

/* Simple values.
 * --------------
 */

void rt_val_push_bool(struct Stack *stack, VAL_BOOL_T value);
void rt_val_push_char(struct Stack *stack, VAL_CHAR_T value);
void rt_val_push_int(struct Stack *stack, VAL_INT_T value);
void rt_val_push_real(struct Stack *stack, VAL_REAL_T value);
void rt_val_push_ref(struct Stack *stack, VAL_REF_T value);
void rt_val_push_unit(struct Stack *stack);

/* Compound values.
 * ----------------
 */

void rt_val_push_array_init(struct Stack *stack, VAL_LOC_T *size_loc);
void rt_val_push_tuple_init(struct Stack *stack, VAL_LOC_T *size_loc);
void rt_val_push_cpd_final(
        struct Stack *stack,
        VAL_LOC_T size_loc,
        VAL_SIZE_T size);

void rt_val_push_string(struct Stack *stack, char *begin, char *end);

/* Function values.
 * ----------------
 */

void rt_val_push_func_init(
        struct Stack *stack,
        VAL_LOC_T *size_loc,
        VAL_LOC_T *data_begin,
        VAL_SIZE_T arity,
		enum ValueFuncType type,
        void *impl);

void rt_val_push_func_cap_init_deferred(
        struct Stack *stack,
        VAL_LOC_T *cap_count_loc);

void rt_val_push_func_cap_init(struct Stack *stack, VAL_SIZE_T cap_count);
void rt_val_push_func_cap(struct Stack *stack, char *symbol, VAL_LOC_T loc);
void rt_val_push_func_cap_copy(struct Stack *stack, VAL_LOC_T loc);

void rt_val_push_func_cap_final_deferred(
        struct Stack *stack,
        VAL_LOC_T cap_count_loc,
        VAL_SIZE_T cap_count);

void rt_val_push_func_appl_init(struct Stack *stack, VAL_SIZE_T appl_count);

void rt_val_push_func_final(
        struct Stack *stack,
        VAL_LOC_T size_loc,
        VAL_SIZE_T data_begin);

/* Hacking (poking) API.
 * =====================
 */

void rt_val_poke_bool(struct Stack *stack, VAL_LOC_T loc, VAL_BOOL_T value);
void rt_val_poke_ref(struct Stack *stack, VAL_LOC_T dst, VAL_LOC_T src);
void rt_val_poke_copy(struct Stack *stack, VAL_LOC_T dst, VAL_LOC_T src);

/* Reading (peeking) API.
 * ======================
 */

/** Peeks the header of a value at the given location. */
struct ValueHeader rt_val_peek_header(struct Stack *stack, VAL_LOC_T location);

/** Renders a string from a value. */
void rt_val_to_string(struct Runtime *rt, VAL_LOC_T loc, char **str);

/** Prints a value at a given location. */
void rt_val_print(struct Runtime *rt, VAL_LOC_T loc, bool annotate);

/** Checks whether a value is a string i.e. an array of characters. */
bool rt_val_is_string(struct Runtime *rt, VAL_LOC_T loc);

/* Value iteration.
 * ----------------
 */

/** Counts the compount value elements and returns the value. */
int rt_val_cpd_len(struct Runtime *rt, VAL_LOC_T location);

/**
 * Advance location by:
 * - value header = VAL_HEAD_BYTES
 * - value size   = value.header.size
 */
VAL_LOC_T rt_val_next_loc(struct Runtime *rt, VAL_LOC_T loc);

/* Value meta-date evaluation.
 * ---------------------------
 */

/** Peek the type of the value at the given location. */
enum ValueType rt_val_peek_type(struct Stack *stack, VAL_LOC_T loc);

/** Peek the size of the value at the given location. */
VAL_SIZE_T rt_val_peek_size(struct Stack *stack, VAL_LOC_T loc);

/* Actual values reading.
 * ----------------------
 */

/** Peek a boolean value at the given location. */
VAL_BOOL_T rt_val_peek_bool(struct Runtime *rt, VAL_LOC_T loc);

/** Peek a character value at the given location. */
VAL_CHAR_T rt_val_peek_char(struct Runtime *rt, VAL_LOC_T loc);

/** Peek a integer value at the given location. */
VAL_INT_T rt_val_peek_int(struct Runtime *rt, VAL_LOC_T loc);

/** Peek a real value at the given location. */
VAL_REAL_T rt_val_peek_real(struct Runtime *rt, VAL_LOC_T loc);

/** Returns the location pointed by the reference. */
VAL_LOC_T rt_val_peek_ref(struct Runtime *rt, VAL_LOC_T loc);

/** Returns the location of the first element of the compound value. */
VAL_LOC_T rt_val_cpd_first_loc(VAL_LOC_T loc);

/**
 * Peek an array of char as a string.
 * NOTE that the client is responsible for releasing the string buffer.
 */
char* rt_val_peek_cpd_as_string(struct Runtime *rt, VAL_LOC_T loc);

/** Computes the relevant locations of a function value. */
struct ValueFuncData rt_val_function_data(struct Runtime *rt, VAL_LOC_T loc);

/** Peek a function capture symbol. */
char *rt_val_peek_fun_cap_symbol(struct Runtime *rt, VAL_LOC_T cap_loc);

/** Return the location of the value captured by the capture at the location. */
VAL_LOC_T rt_val_fun_cap_loc(struct Runtime *rt, VAL_LOC_T cap_loc);

/** Get the location of the next capture. */
VAL_LOC_T rt_val_fun_next_cap_loc(struct Runtime *rt, VAL_LOC_T loc);

/** Get the location of the next applied argument. */
VAL_LOC_T rt_val_fun_next_appl_loc(struct Runtime *rt, VAL_LOC_T loc);

/* Integrity assertion.
 * ====================
 */

bool rt_val_pair_homo(struct Runtime *rt, VAL_LOC_T x, VAL_LOC_T y);
bool rt_val_compound_homo(struct Runtime *rt, VAL_LOC_T val_loc);
bool rt_val_pair_equal_mempattern(struct Runtime *rt, VAL_LOC_T x, VAL_LOC_T y);

/* Utility functions.
 * ==================
 */

bool rt_val_eq_rec(struct Runtime *rt, VAL_LOC_T x, VAL_LOC_T y);
bool rt_val_eq_bin(struct Runtime *rt, VAL_LOC_T x, VAL_LOC_T y);
bool rt_val_string_eq(struct Runtime *rt, VAL_LOC_T loc, char *str);

#endif
