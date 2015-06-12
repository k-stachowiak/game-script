/* Copyright (C) 2015 Krzysztof Stachowiak */

#ifndef MNDETAIL_H
#define MNDETAIL_H

#include "moon.h"
#include "rt_val.h"

struct MoonValue *mn_make_api_value(struct Runtime *rt, VAL_LOC_T loc);
struct MoonValue *mn_make_api_value_compound(struct Runtime *rt, VAL_LOC_T loc);
void mn_api_value_free(struct MoonValue *value);

#endif
