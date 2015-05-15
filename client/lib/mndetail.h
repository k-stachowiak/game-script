/* Copyright (C) 2015 Krzysztof Stachowiak */

#ifndef MNDETAIL_H
#define MNDETAIL_H

#include "moon.h"
#include "rt_val.h"

extern struct Runtime *runtime;

struct MoonValue *make_api_value(VAL_LOC_T loc);
struct MoonValue *make_api_value_compound(VAL_LOC_T loc);
void api_value_free(struct MoonValue *value);

#endif
