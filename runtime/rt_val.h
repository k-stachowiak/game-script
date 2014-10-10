/* Copyright (C) 2014 Krzysztof Stachowiak */

#ifndef RT_VAL_H
#define RT_VAL_H

#include "runtime.h"

VAL_LOC_T rt_val_next_loc(struct Runtime *rt, VAL_LOC_T loc);

enum ValueType rt_val_peek_type(struct Runtime *rt, VAL_LOC_T loc);
VAL_SIZE_T rt_val_peek_size(struct Runtime *rt, VAL_LOC_T loc);

VAL_BOOL_T rt_val_peek_bool(struct Runtime *rt, VAL_LOC_T loc);
VAL_CHAR_T rt_val_peek_char(struct Runtime *rt, VAL_LOC_T loc);
VAL_INT_T rt_val_peek_int(struct Runtime *rt, VAL_LOC_T loc);
VAL_REAL_T rt_val_peek_real(struct Runtime *rt, VAL_LOC_T loc);
char* rt_val_peek_string(struct Runtime *rt, VAL_LOC_T loc);

VAL_LOC_T rt_val_cpd_first_loc(VAL_LOC_T loc);

void rt_val_function_locs(
        struct Runtime *rt, 
        VAL_LOC_T loc,
        VAL_LOC_T *impl_loc,
        VAL_LOC_T *cap_start,
        VAL_LOC_T *appl_start);

char *rt_val_peek_fun_cap_symbol(struct Runtime *rt, VAL_LOC_T cap_loc);
VAL_LOC_T rt_val_fun_cap_loc(struct Runtime *rt, VAL_LOC_T cap_loc);
VAL_LOC_T rt_val_fun_next_cap_loc(struct Runtime *rt, VAL_LOC_T loc);
VAL_LOC_T rt_val_fun_next_appl_loc(struct Runtime *rt, VAL_LOC_T loc);

#endif
