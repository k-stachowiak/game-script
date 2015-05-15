/* Copyright (C) 2015 Krzysztof Stachowiak */

#include "collection.h"
#include "error.h"
#include "mndetail.h"

struct ClifMapKvp {
	char *key;
	ClifHandler handler;
	struct ClifMapKvp *next;
};

struct ClifMap {
	struct ClifMapKvp *map;
	struct ClifMapKvp *end;
} clif_map;

static void clif_error_already_inserted(char *symbol)
{
    struct ErrMessage msg;
    err_msg_init(&msg, "LIB");
    err_msg_append(&msg, "Symbol \"%s\" already inserted", symbol);
    err_msg_set(&msg);
}

void clif_init(void)
{
	clif_map.map = NULL;
	clif_map.end = NULL;
}

void clif_deinit(void)
{
	struct ClifMapKvp *temp, *kvp = clif_map.map;
	while (kvp) {
		temp = kvp;
		kvp = kvp->next;
		mem_free(temp->key);
		mem_free(temp);
	}
	clif_map.map = NULL;
	clif_map.end = NULL;
}

void clif_register(char *symbol, ClifHandler handler)
{
    struct ClifMapKvp *kvp = clif_map.map;
    int len = strlen(symbol);
    char *symbol_copy = mem_malloc(len + 1);
    memcpy(symbol_copy, symbol, len + 1);

    while (kvp) {
		if (strcmp(kvp->key, symbol) == 0) {
			clif_error_already_inserted(symbol);
			return;
		}
    }

    kvp = mem_malloc(sizeof(*kvp));
    kvp->key = symbol_copy;
    kvp->handler = handler;
    kvp->next = NULL;

    LIST_APPEND(kvp, &clif_map.map, &clif_map.end);
}

void clif_common_handler(char *symbol, VAL_LOC_T *arg_locs, int arg_count)
{
}

