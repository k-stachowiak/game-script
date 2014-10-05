/* Copyright (C) 2014 Krzysztof Stachowiak */

#ifndef RT_H
#define RT_H

#include <stdbool.h>

#include "value.h"

void rt_init(void);
void rt_deinit(void);

void rt_save(void);
void rt_restore(void);

VAL_LOC_T rt_current_top(void);

void rt_consume_one(struct AstNode *ast, VAL_LOC_T *loc, struct AstNode **next);
bool rt_consume_list(struct AstNode *ast);

void rt_for_each_stack_val(void(*f)(VAL_LOC_T, struct Value*));
void rt_for_each_sym(void(*f)(char*, VAL_LOC_T));
struct Value rt_peek(VAL_LOC_T loc);

void rt_val_print(VAL_LOC_T loc, bool annotate);
VAL_LOC_T rt_next_loc(VAL_LOC_T loc);

enum ValueType rt_val_type(VAL_LOC_T loc);
VAL_SIZE_T rt_val_size(VAL_LOC_T loc);

VAL_SIZE_T rt_peek_size(VAL_LOC_T loc);
void *rt_peek_ptr(VAL_LOC_T loc);

VAL_BOOL_T rt_peek_val_bool(VAL_LOC_T loc);
VAL_CHAR_T rt_peek_val_char(VAL_LOC_T loc);
VAL_INT_T rt_peek_val_int(VAL_LOC_T loc);
VAL_REAL_T rt_peek_val_real(VAL_LOC_T loc);
char* rt_peek_val_string(VAL_LOC_T loc);

// int rt_peek_val_cpd_size(VAL_LOC_T loc);
VAL_LOC_T rt_peek_val_cpd_first(VAL_LOC_T loc);

void rt_peek_val_fun_locs(
        VAL_LOC_T loc,
        VAL_LOC_T *impl_loc,
        VAL_LOC_T *cap_start,
        VAL_LOC_T *appl_start);


#endif
