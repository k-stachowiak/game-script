/* Copyright (C) 2014 Krzysztof Stachowiak */

#ifndef BIF_DETAIL_H
#define BIF_DETAIL_H

#include <stdbool.h>

#include "common.h"

extern bool initialized;
extern struct Location bif_location;
extern struct Location bif_arg_locations[];
extern char *bif_arg_names[];

void bif_init_arythmetic(void);
void bif_init_compare(void);

#endif