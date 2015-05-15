/* Copyright (C) 2015 Krzysztof Stachowiak */

#ifndef CLIF_H
#define CLIF_H

#include "moon.h"

void clif_init(void);
void clif_deinit(void);
void clif_register(char *symbol, ClifHandler handler);
void clif_common_handler(char *symbol, VAL_LOC_T *arg_locs, int arg_count);

#endif

