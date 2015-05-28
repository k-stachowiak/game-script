/* Copyright (C) 2014,2015 Krzysztof Stachowiak */

#include <stdbool.h>
#include <math.h>

#include "log.h"
#include "eval.h"
#include "bif.h"
#include "bif_detail.h"
#include "runtime.h"
#include "error.h"
#include "memory.h"
#include "rt_val.h"

#define BIF_ARYTHM_UNARY_DEF(NAME) \
    void NAME(struct Runtime *rt, VAL_LOC_T x_loc) \
    { \
        VAL_REAL_T r; \
        VAL_INT_T i; \
        switch (bif_match_un(rt, x_loc, &i, &r)) { \
        case VAL_INT: \
            rt_val_push_int(&rt->stack, NAME##_impl_int(i)); \
            break; \
        case VAL_REAL: \
            rt_val_push_real(&rt->stack, NAME##_impl_real(r)); \
            break; \
        default: \
			err_push("BIF", "Arguments of arythmetic BIF must be of equal numeric type"); \
            break; \
        } \
    }

#define BIF_ARYTHM_BINARY_DEF(NAME) \
    void NAME(struct Runtime *rt, VAL_LOC_T x_loc, VAL_LOC_T y_loc) \
    { \
        VAL_REAL_T rx, ry; \
        VAL_INT_T ix, iy; \
        switch (bif_match_bin(rt, x_loc, y_loc, &ix, &iy, &rx, &ry)) { \
        case BBM_BOTH_INT: \
            rt_val_push_int(&rt->stack, NAME##_impl_int(ix, iy)); \
            break; \
        case BBM_BOTH_REAL: \
        case BBM_HETERO: \
            rt_val_push_real(&rt->stack, NAME##_impl_real(rx, ry)); \
            break; \
        case BBM_MISMATCH: \
			err_push("BIF", "Arguments of arythmetic BIF must be of equal numeric type"); \
            break; \
        } \
    }

#define BIF_TRUNCATING(NAME) \
    void NAME(struct Runtime *rt, VAL_LOC_T x_loc) \
    { \
        VAL_REAL_T x; \
        if (rt_val_peek_type(rt, x_loc) != VAL_REAL) { \
			err_push("BIF", "Arguments of arythmetic BIF must be of equal numeric type"); \
            return; \
        } \
        x = rt_val_peek_real(rt, x_loc); \
        rt_val_push_int(&rt->stack, NAME##_impl(x)); \
    }

static VAL_INT_T bif_add_impl_int(VAL_INT_T x, VAL_INT_T y) { return x + y; }
static VAL_INT_T bif_sub_impl_int(VAL_INT_T x, VAL_INT_T y) { return x - y; }
static VAL_INT_T bif_mul_impl_int(VAL_INT_T x, VAL_INT_T y) { return x * y; }
static VAL_INT_T bif_div_impl_int(VAL_INT_T x, VAL_INT_T y) { return x / y; }
static VAL_INT_T bif_mod_impl_int(VAL_INT_T x, VAL_INT_T y) { return x % y; }
static VAL_REAL_T bif_add_impl_real(VAL_REAL_T x, VAL_REAL_T y) { return x + y; }
static VAL_REAL_T bif_sub_impl_real(VAL_REAL_T x, VAL_REAL_T y) { return x - y; }
static VAL_REAL_T bif_mul_impl_real(VAL_REAL_T x, VAL_REAL_T y) { return x * y; }
static VAL_REAL_T bif_div_impl_real(VAL_REAL_T x, VAL_REAL_T y) { return x / y; }
static VAL_REAL_T bif_mod_impl_real(VAL_REAL_T x, VAL_REAL_T y) { return fmod(x, y); }

static VAL_INT_T bif_sqrt_impl_int(VAL_INT_T x) { return (VAL_INT_T)sqrt((double)x); }
static VAL_REAL_T bif_sqrt_impl_real(VAL_REAL_T x) { return sqrt(x); }

BIF_ARYTHM_BINARY_DEF(bif_add)
BIF_ARYTHM_BINARY_DEF(bif_sub)
BIF_ARYTHM_BINARY_DEF(bif_mul)
BIF_ARYTHM_BINARY_DEF(bif_div)
BIF_ARYTHM_BINARY_DEF(bif_mod)
BIF_ARYTHM_UNARY_DEF(bif_sqrt)

void bif_real(struct Runtime *rt, VAL_LOC_T x_loc)
{
	VAL_INT_T x;
	if (rt_val_peek_type(rt, x_loc) != VAL_INT) {
		err_push("BIF", "Arguments of expanding arythmetic BIF must be of integer type");
		return;
	}
	x = rt_val_peek_int(rt, x_loc);
	rt_val_push_real(&rt->stack, (VAL_REAL_T)x);
}

static VAL_INT_T bif_floor_impl(VAL_REAL_T x) { return (VAL_INT_T)floor(x); }
static VAL_INT_T bif_ceil_impl(VAL_REAL_T x) { return (VAL_INT_T)ceil(x); }
static VAL_INT_T bif_round_impl(VAL_REAL_T x) { return (VAL_INT_T)round(x); }

BIF_TRUNCATING(bif_floor)
BIF_TRUNCATING(bif_ceil)
BIF_TRUNCATING(bif_round)
