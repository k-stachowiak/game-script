/* Copyright (C) 2015 Krzysztof Stachowiak */

#ifndef CLIF_H
#define CLIF_H

#include "moon.h"

struct ClifMapKvp {
	char *key;
	ClifHandler handler;
	struct ClifMapKvp *next;
};

struct ClifMap {
	struct ClifMapKvp *map;
	struct ClifMapKvp *end;
} clif_map;

struct ClifMap *clif_make(void);
void clif_free(struct ClifMap *cm);
void clif_register(struct ClifMap *cm, char *symbol, ClifHandler handler);
void clif_common_handler(struct Runtime *rt, char *symbol, VAL_LOC_T *arg_locs, int arg_count);

extern struct SourceLocation clif_location;

#endif

