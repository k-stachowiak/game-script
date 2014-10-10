/* Copyright (C) 2014 Krzysztof Stachowiak */

#ifndef RT_VAL_H
#define RT_VAL_H

#include "runtime.h"

/* Data structores.
 * ================
 */

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

/* Writing (pushing) API.
 * ======================
 */

/* Simple values.
 * --------------
 */

void rt_val_push_bool(struct Stack *stack, VAL_BOOL_T value);
void rt_val_push_char(struct Stack *stack, VAL_CHAR_T value);
void rt_val_push_int(struct Stack *stack, VAL_INT_T value);
void rt_val_push_real(struct Stack *stack, VAL_REAL_T value);
void rt_val_push_string(struct Stack *stack, char *value);

/* Compound values.
 * ----------------
 */

void rt_val_push_array_init(struct Stack *stack, VAL_LOC_T *size_loc);
void rt_val_push_tuple_init(struct Stack *stack, VAL_LOC_T *size_loc);
void rt_val_push_cpd_final(
        struct Stack *stack,
        VAL_LOC_T size_loc,
        VAL_SIZE_T size);

/* Function values.
 * ----------------
 */

void rt_val_push_func_init(
        struct Stack *stack,
        VAL_LOC_T *size_loc,
        VAL_LOC_T *data_begin,
        void *func_def);

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
void rt_val_push_func_appl_empty(struct Stack *stack);

void rt_val_push_func_final(
        struct Stack *stack,
        VAL_LOC_T size_loc,
        VAL_SIZE_T data_begin);

/* Reading (peeking) API.
 * ======================
 */

/** Peeks the header of a value at the given location. */
struct ValueHeader rt_val_peek_header(struct Stack *stack, VAL_LOC_T location);

/** Prints a value at a given location. */
void rt_val_print(struct Runtime *rt, VAL_LOC_T loc, bool annotate);

/* Value iteration.
 * ----------------
 */

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
enum ValueType rt_val_peek_type(struct Runtime *rt, VAL_LOC_T loc);

/** Peek the size of the value at the given location. */
VAL_SIZE_T rt_val_peek_size(struct Runtime *rt, VAL_LOC_T loc);

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

/** Peek a string value at the given location. */
char* rt_val_peek_string(struct Runtime *rt, VAL_LOC_T loc);

/** Returns the location of the first element of the compound value. */
VAL_LOC_T rt_val_cpd_first_loc(VAL_LOC_T loc);

/** Computes the relevant locations of a function value. */
void rt_val_function_locs(
        struct Runtime *rt, 
        VAL_LOC_T loc,
        VAL_LOC_T *impl_loc,
        VAL_LOC_T *cap_start,
        VAL_LOC_T *appl_start);

/** Peek a function capture symbol. */
char *rt_val_peek_fun_cap_symbol(struct Runtime *rt, VAL_LOC_T cap_loc);

/** Return the location of the value captured by the capture at the location. */
VAL_LOC_T rt_val_fun_cap_loc(struct Runtime *rt, VAL_LOC_T cap_loc);

/** Get the location of the next capture. */
VAL_LOC_T rt_val_fun_next_cap_loc(struct Runtime *rt, VAL_LOC_T loc);

/** Get the location of the next applied argument. */
VAL_LOC_T rt_val_fun_next_appl_loc(struct Runtime *rt, VAL_LOC_T loc);

#endif
