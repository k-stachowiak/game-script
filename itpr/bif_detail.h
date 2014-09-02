/* Copyright (C) 2014 Krzysztof Stachowiak */

#ifndef BIF_DETAIL_H
#define BIF_DETAIL_H

#include <stdbool.h>

#include "common.h"

enum BifBinaryMatch {
	BBM_BOTH_INT,
	BBM_BOTH_REAL,
	BBM_MISMATCH
};

extern bool initialized;
extern struct Location bif_location;
extern struct Location bif_arg_locations[];
extern char *bif_arg_names[];

void bif_init_impl_ptrs(struct AstNode *node);

enum ValueType bif_match_un(
	struct Stack *stack,
	VAL_LOC_T x_loc,
	VAL_INT_T *i,
	VAL_REAL_T *r);

enum BifBinaryMatch bif_match_bin(
	struct Stack *stack,
	VAL_LOC_T x_loc, VAL_LOC_T y_loc,
	VAL_INT_T *ix, VAL_INT_T *iy,
	VAL_REAL_T *rx, VAL_REAL_T *ry);

void bif_init_arythmetic(void);
void bif_init_compare(void);
void bif_init_array(void);

#endif