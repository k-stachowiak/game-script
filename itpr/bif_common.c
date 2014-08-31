/* Copyright (C) 2014 Krzysztof Stachowiak */

#include "bif.h"

bool initialized = false;
struct Location bif_location = { -1, -1 };
struct Location bif_arg_locations[] = { { -1, -1 }, { -1, -1 } };
char *bif_arg_names[] = { "a", "b" };

void bif_assure_init(void)
{
	if (initialized) {
		return;
	}

	initialized = true;
	bif_init_arythmetic();
	bif_init_compare();
}