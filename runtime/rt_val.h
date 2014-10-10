/* Copyright (C) 2014 Krzysztof Stachowiak */

#ifndef RT_VAL_H
#define RT_VAL_H

#include "runtime.h"

/* Reading (peeking) API.
 * ======================
 */

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
