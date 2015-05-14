/* Copyright (C) 2015 Krzysztof Stachowiak */

#ifndef CPPRAND_H
#define CPPRAND_H

#include "rt_val.h"

#ifdef __cplusplus
extern "C" {
#endif

VAL_INT_T cpprand_ui(VAL_INT_T lo, VAL_INT_T hi);
VAL_REAL_T cpprand_ur(VAL_REAL_T lo, VAL_REAL_T hi);
VAL_BOOL_T cpprand_ber(VAL_REAL_T p);
VAL_REAL_T cpprand_exp(VAL_REAL_T l);
VAL_REAL_T cpprand_gauss(VAL_REAL_T u, VAL_REAL_T s);
VAL_INT_T cpprand_distr(VAL_REAL_T *dens_values, int dens_count);

#ifdef __cplusplus
}
#endif

#endif
