/* Copyright (C) 2015 Krzysztof Stachowiak */

#include <random>
#include <string>
#include <vector>

#include "cpprand.h"

namespace {

std::random_device device;
std::mt19937 generator(device());

}

extern "C" {

VAL_INT_T cpprand_ui(VAL_INT_T lo, VAL_INT_T hi)
{
    std::uniform_int_distribution<VAL_INT_T> distribution(lo, hi);
	return distribution(generator);
}

VAL_REAL_T cpprand_ur(VAL_REAL_T lo, VAL_REAL_T hi)
{
    std::uniform_real_distribution<VAL_REAL_T> distribution(lo, hi);
	return distribution(generator);
}

VAL_BOOL_T cpprand_ber(VAL_REAL_T p)
{
    std::bernoulli_distribution distribution(p);
	return distribution(generator);
}

VAL_REAL_T cpprand_exp(VAL_REAL_T l)
{
    std::exponential_distribution<VAL_REAL_T> distribution(l);
	return distribution(generator);
}

VAL_REAL_T cpprand_gauss(VAL_REAL_T u, VAL_REAL_T s)
{
    std::normal_distribution<VAL_REAL_T> distribution(u, s);
	return distribution(generator);
}

VAL_INT_T cpprand_distr(VAL_REAL_T *dens_values, int dens_count)
{
    std::discrete_distribution<VAL_INT_T> distribution(dens_values, dens_values + dens_count);
	return distribution(generator);
}


}

