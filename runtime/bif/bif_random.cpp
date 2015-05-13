/* Copyright (C) 2015 Krzysztof Stachowiak */

#include <random>
#include <string>
#include <vector>

#include "error.h"
#include "eval.h"
#include "rt_val.h"

namespace {

static void bif_rand_error_arg(int arg, const std::string& func, const std::string& condition)
{
    struct ErrMessage msg;
    err_msg_init_src(&msg, (char*)"BIF EVAL RANDOM", eval_location_top());
    err_msg_append(&msg, "Argument %d of _%s_ %s", arg, func.c_str(), condition.c_str());
    err_msg_set(&msg);
}

std::random_device device;
std::mt19937 generator(device());

}

extern "C" {

void bif_rand_ui(Runtime *rt, VAL_LOC_T lo_loc, VAL_LOC_T hi_loc)
{
    ValueType lo_type = rt_val_peek_type(rt, lo_loc);
    ValueType hi_type = rt_val_peek_type(rt, hi_loc);

    if (lo_type != VAL_INT) {
        bif_rand_error_arg(1, "rand_ui", "must be an integer");
        return;
    }

    if (hi_type != VAL_INT) {
        bif_rand_error_arg(2, "rand_ui", "must be an integer");
        return;
    }

    VAL_INT_T lo = rt_val_peek_int(rt, lo_loc);
    VAL_INT_T hi = rt_val_peek_int(rt, hi_loc);

    std::uniform_int_distribution<VAL_INT_T> distribution(lo, hi);

    rt_val_push_int(rt->stack, distribution(generator));
}

void bif_rand_ur(Runtime *rt, VAL_LOC_T lo_loc, VAL_LOC_T hi_loc)
{
    ValueType lo_type = rt_val_peek_type(rt->stack, lo_loc);
    ValueType hi_type = rt_val_peek_type(rt->stack, hi_loc);

    if (lo_type != VAL_REAL) {
        bif_rand_error_arg(1, "rand_ur", "must be a real value");
        return;
    }

    if (hi_type != VAL_REAL) {
        bif_rand_error_arg(2,"rand_ur", "must be a real value");
        return;
    }

    VAL_REAL_T lo = rt_val_peek_real(rt, lo_loc);
    VAL_REAL_T hi = rt_val_peek_real(rt, hi_loc);

    std::uniform_int_distribution<VAL_REAL_T> distribution(lo, hi);

    rt_val_push_real(rt->stack, distribution(generator));
}

void bif_rand_ber(Runtime *rt, VAL_LOC_T p_loc)
{
    ValueType p_type = rt_val_peek_type(rt->stack, p_loc);
    if (p_type != VAL_REAL) {
        bif_rand_error_arg(1, "rand_ber", "must be a real value");
        return;
    }
    VAL_REAL_T p = rt_val_peek_real(rt, p_loc);
    std::bernoulli_distribution distribution(p);
    rt_val_push_real(rt->stack, distribution(generator));
}

void bif_rand_exp(Runtime *rt, VAL_LOC_T l_loc)
{
    ValueType l_type = rt_val_peek_type(rt->stack, l_loc);
    if (l_type != VAL_REAL) {
        bif_rand_error_arg(1, "rand_exp", "must be a real value");
        return;
    }
    VAL_REAL_T l = rt_val_peek_real(rt, l_loc);
    std::exponential_distribution distribution(l);
    rt_val_push_real(rt->stack, distribution(generator));
}

void bif_rand_gauss(Runtime *rt, VAL_LOC_T u_loc, VAL_LOC_T s_loc)
{
    Valuetype u_type = rt_val_peek_type(rt->stack, u_loc);
    Valuetype s_type = rt_val_peek_type(rt->stack, s_loc);

    if (u_type != VAL_REAL) {
        bif_rand_error_arg(1, "rand_gauss", "must be a real value");
        return;
    }

    if (s_type != VAL_REAL) {
        bif_rand_error_arg(2,"rand_gauss", "must be a real value");
        return;
    }

    VAL_REAL_T u = rt_val_peek_real(rt, u_loc);
    VAL_REAL_T s = rt_val_peek_real(rt, s_loc);

    std::normal_distribution<VAL_REAL_T> distribution(u, s);

    rt_val_push_real(rt->stack, distribution(generator));
}

void bif_rand_distr(Runtime *rt, VAL_LOC_T d_loc)
{
    ValueType d_type = rt_val_peek_type(rt->stack, d_loc);
    if (d_type != VAL_ARRAY) {
        bif_rand_error_arg(1, "rand_distr", "must be an array");
        return;
    }

    VAL_SIZE_T len = rt_val_cpd_len(rt, d_loc);
    if (len == 0) {
        bif_rand_error_arg(1, "rand_distr", "must not be empty");
        return;
    }

    VAL_LOC_T loc = rt_val_cpd_first_loc(d_loc);
    ValueType first_type = rt_val_peek_type(rt->stack, loc);
    if (first_type != VAL_REAL) {
        bif_rand_error_arg(1, "rand_distr", "must containt real values");
        return;
    }

    std::vector<VAL_REAL_T> distribution(len);
    for (VAL_SIZE_T i = 0; i < len; ++i) {
        distribution.push_back(rt_val_peek_real(rt->stack, loc));
        loc = rt_val_next_loc(rt, loc);
    }

    std::discrete_distribution<>(begin(distribution), end(distribution));

    rt_val_push_int(rt->stack, distribution(generator));
}

}
