/* Copyright (C) 2014,2015 Krzysztof Stachowiak */

#ifndef BIF_H
#define BIF_H

#include "define.h"

#define BIF_MAX_ARITY 3

struct Runtime;

typedef void (*bif_unary_func)(struct Runtime *rt, VAL_LOC_T x_loc);
typedef void (*bif_binary_func)(struct Runtime *rt, VAL_LOC_T x_loc, VAL_LOC_T y_loc);
typedef void (*bif_ternary_func)(struct Runtime *rt, VAL_LOC_T x_loc, VAL_LOC_T y_loc, VAL_LOC_T z_loc);

extern struct SourceLocation bif_location;

/* Arythmetic */
void bif_sqrt(struct Runtime *rt, VAL_LOC_T x_loc);
void bif_add(struct Runtime *rt, VAL_LOC_T x_loc, VAL_LOC_T y_loc);
void bif_sub(struct Runtime *rt, VAL_LOC_T x_loc, VAL_LOC_T y_loc);
void bif_mul(struct Runtime *rt, VAL_LOC_T x_loc, VAL_LOC_T y_loc);
void bif_div(struct Runtime *rt, VAL_LOC_T x_loc, VAL_LOC_T y_loc);
void bif_mod(struct Runtime *rt, VAL_LOC_T x_loc, VAL_LOC_T y_loc);
void bif_floor(struct Runtime *rt, VAL_LOC_T x_loc);
void bif_ceil(struct Runtime *rt, VAL_LOC_T x_loc);
void bif_round(struct Runtime *rt, VAL_LOC_T x_loc);

/* Comparison */
void bif_eq(struct Runtime *rt, VAL_LOC_T x_loc, VAL_LOC_T y_loc);
void bif_lt(struct Runtime *rt, VAL_LOC_T x_loc, VAL_LOC_T y_loc);

/* Logic */
void bif_and(struct Runtime *rt, VAL_LOC_T x_loc, VAL_LOC_T y_loc);
void bif_or(struct Runtime *rt, VAL_LOC_T x_loc, VAL_LOC_T y_loc);
void bif_xor(struct Runtime *rt, VAL_LOC_T x_loc, VAL_LOC_T y_loc);
void bif_not(struct Runtime *rt, VAL_LOC_T x_loc);

/* Compound */
void bif_length(struct Runtime* rt, VAL_LOC_T x_loc);
void bif_push_front(struct Runtime *rt, VAL_LOC_T x_loc, VAL_LOC_T y_loc);
void bif_reverse(struct Runtime* rt, VAL_LOC_T x_loc);
void bif_cat(struct Runtime* rt, VAL_LOC_T x_loc, VAL_LOC_T y_loc);
void bif_at(struct Runtime* rt, VAL_LOC_T x_loc, VAL_LOC_T y_loc);
void bif_slice(struct Runtime *rt, VAL_LOC_T x_loc, VAL_LOC_T y_loc, VAL_LOC_T z_loc);

/* Text */
void bif_print(struct Runtime *rt, VAL_LOC_T str_loc);
void bif_format(struct Runtime *rt, VAL_LOC_T fmt_loc, VAL_LOC_T args_loc);
void bif_to_string(struct Runtime *rt, VAL_LOC_T arg_loc);
void bif_parse(struct Runtime *rt, VAL_LOC_T arg_loc);
void bif_parse_bool(struct Runtime *rt, VAL_LOC_T arg_loc);
void bif_parse_char(struct Runtime *rt, VAL_LOC_T arg_loc);
void bif_parse_int(struct Runtime *rt, VAL_LOC_T arg_loc);
void bif_parse_real(struct Runtime *rt, VAL_LOC_T arg_loc);

#endif
