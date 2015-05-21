/* Copyright (C) 2015 Krzysztof Stachowiak */

#include "error.h"
#include "eval.h"
#include "rt_val.h"
#include "cpprand.h"

static void bif_rand_error_arg(int arg, char *func, char *condition)
{
	err_push("BIF", *eval_location_top(), "Argument %d of _%s_ %s", arg, func, condition);
}

void bif_rand_ui(struct Runtime *rt, VAL_LOC_T lo_loc, VAL_LOC_T hi_loc)
{
    enum ValueType lo_type = rt_val_peek_type(rt, lo_loc);
    enum ValueType hi_type = rt_val_peek_type(rt, hi_loc);
    VAL_INT_T lo;
    VAL_INT_T hi;

    if (lo_type != VAL_INT) {
        bif_rand_error_arg(1, "rand_ui", "must be an integer");
        return;
    }

    if (hi_type != VAL_INT) {
        bif_rand_error_arg(2, "rand_ui", "must be an integer");
        return;
    }

    lo = rt_val_peek_int(rt, lo_loc);
    hi = rt_val_peek_int(rt, hi_loc);

    rt_val_push_int(rt->stack, cpprand_ui(lo, hi));
}

void bif_rand_ur(struct Runtime *rt, VAL_LOC_T lo_loc, VAL_LOC_T hi_loc)
{
    enum ValueType lo_type = rt_val_peek_type(rt, lo_loc);
    enum ValueType hi_type = rt_val_peek_type(rt, hi_loc);
    VAL_REAL_T lo;
    VAL_REAL_T hi;

    if (lo_type != VAL_REAL) {
        bif_rand_error_arg(1, "rand_ur", "must be a real value");
        return;
    }

    if (hi_type != VAL_REAL) {
        bif_rand_error_arg(2,"rand_ur", "must be a real value");
        return;
    }

    lo = rt_val_peek_real(rt, lo_loc);
    hi = rt_val_peek_real(rt, hi_loc);

    rt_val_push_real(rt->stack, cpprand_ur(lo, hi));
}

void bif_rand_ber(struct Runtime *rt, VAL_LOC_T p_loc)
{
    enum ValueType p_type = rt_val_peek_type(rt, p_loc);
    VAL_REAL_T p;
    if (p_type != VAL_REAL) {
        bif_rand_error_arg(1, "rand_ber", "must be a real value");
        return;
    }
    p = rt_val_peek_real(rt, p_loc);
    rt_val_push_bool(rt->stack, cpprand_ber(p));
}

void bif_rand_exp(struct Runtime *rt, VAL_LOC_T l_loc)
{
    enum ValueType l_type = rt_val_peek_type(rt, l_loc);
    VAL_REAL_T l;
    if (l_type != VAL_REAL) {
        bif_rand_error_arg(1, "rand_exp", "must be a real value");
        return;
    }
    l = rt_val_peek_real(rt, l_loc);
    rt_val_push_real(rt->stack, cpprand_exp(l));
}

void bif_rand_gauss(struct Runtime *rt, VAL_LOC_T u_loc, VAL_LOC_T s_loc)
{
    enum ValueType u_type = rt_val_peek_type(rt, u_loc);
    enum ValueType s_type = rt_val_peek_type(rt, s_loc);
    VAL_REAL_T u;
    VAL_REAL_T s;

    if (u_type != VAL_REAL) {
        bif_rand_error_arg(1, "rand_gauss", "must be a real value");
        return;
    }

    if (s_type != VAL_REAL) {
        bif_rand_error_arg(2,"rand_gauss", "must be a real value");
        return;
    }

    u = rt_val_peek_real(rt, u_loc);
    s = rt_val_peek_real(rt, s_loc);

    rt_val_push_real(rt->stack, cpprand_gauss(u, s));
}

void bif_rand_distr(struct Runtime *rt, VAL_LOC_T d_loc)
{
    enum ValueType d_type = rt_val_peek_type(rt, d_loc);
    enum ValueType first_type;
	VAL_SIZE_T len, i;
	VAL_LOC_T loc;
	VAL_REAL_T *density;
    VAL_INT_T result;

    if (d_type != VAL_ARRAY) {
        bif_rand_error_arg(1, "rand_distr", "must be an array");
        return;
    }

    len = rt_val_cpd_len(rt, d_loc);
    if (len == 0) {
        bif_rand_error_arg(1, "rand_distr", "must not be empty");
        return;
    }

    loc = rt_val_cpd_first_loc(d_loc);
    first_type = rt_val_peek_type(rt, loc);
    if (first_type != VAL_REAL) {
        bif_rand_error_arg(1, "rand_distr", "must containt real values");
        return;
    }

	density = mem_malloc(len * sizeof(*density));
    for (i = 0; i < len; ++i) {
        density[i] = rt_val_peek_real(rt, loc);
        loc = rt_val_next_loc(rt, loc);
    }
    result = cpprand_distr(density, len);
	mem_free(density);

    rt_val_push_int(rt->stack, result);
}

