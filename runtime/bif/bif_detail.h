/* Copyright (C) 2014,2015 Krzysztof Stachowiak */

#ifndef BIF_DETAIL_H
#define BIF_DETAIL_H

#include <stdbool.h>

#include "runtime.h"

enum BifBinaryMatch {
    BBM_BOTH_INT,
    BBM_BOTH_REAL,
    BBM_HETERO,
    BBM_MISMATCH
};

enum ValueType bif_match_un(
    struct Runtime *rt,
    VAL_LOC_T x_loc,
    VAL_INT_T *i,
    VAL_REAL_T *r);

enum BifBinaryMatch bif_match_bin(
    struct Runtime *rt,
    VAL_LOC_T x_loc, VAL_LOC_T y_loc,
    VAL_INT_T *ix, VAL_INT_T *iy,
    VAL_REAL_T *rx, VAL_REAL_T *ry);

#endif
