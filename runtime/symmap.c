/* Copyright (C) 2014,2015 Krzysztof Stachowiak */

#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "collection.h"
#include "error.h"
#include "memory.h"
#include "symmap.h"
#include "eval.h"

static void symmap_error_already_inserted(char *symbol)
{
	struct ErrMessage msg;
	err_msg_init_src(&msg, "SYM MAP", eval_location_top());
	err_msg_append(&msg, "Symbol \"%s\" already inserted", symbol);
	err_msg_set(&msg);
}

void sym_map_init_global(struct SymMap *sym_map)
{
	sym_map->global = NULL;
	sym_map->map = NULL;
	sym_map->end = NULL;
}

void sym_map_init_local(
		struct SymMap *sym_map,
        struct SymMap *global)
{
    sym_map->global = global;
    sym_map->map = NULL;
    sym_map->end = NULL;
}

void sym_map_deinit(struct SymMap *sym_map)
{
	struct SymMapKvp *temp, *kvp;
	kvp = sym_map->map;
    while (kvp) {
        temp = kvp;
        kvp = kvp->next;
        mem_free(temp->key);
        mem_free(temp);
    }
    sym_map->map = NULL;
    sym_map->end = NULL;
}

void sym_map_insert(
        struct SymMap *sym_map,
        char *key,
        VAL_LOC_T stack_loc,
        struct SourceLocation *source_loc)
{
    struct SymMapKvp *kvp;
    int len = strlen(key);
	char *key_copy = mem_malloc(len + 1);
    memcpy(key_copy, key, len + 1);

    kvp = sym_map_find_shallow(sym_map, key);
    if (kvp) {
		symmap_error_already_inserted(key);
		return;
    }

	kvp = mem_malloc(sizeof(*kvp));
    kvp->key = key_copy;
    kvp->stack_loc = stack_loc;
    kvp->source_loc = *source_loc;
    kvp->next = NULL;

    LIST_APPEND(kvp, &sym_map->map, &sym_map->end);
}

struct SymMapKvp *sym_map_find(struct SymMap *sym_map, char *key)
{
	struct SymMapKvp *kvp;

	if ((kvp = sym_map_find_shallow(sym_map, key))) {
		return kvp;
	}

    if (sym_map->global) {
        return sym_map_find(sym_map->global, key);
    } else {
        return NULL;
    }
}

struct SymMapKvp *sym_map_find_shallow(struct SymMap *sym_map, char *key)
{
	struct SymMapKvp *kvp = sym_map->map;
	while (kvp) {
		if (strcmp(kvp->key, key) == 0) {
			return kvp;
		}
		kvp = kvp->next;
	}
	return NULL;
}

struct SymMapKvp *sym_map_find_not_global(struct SymMap *sym_map, char *key)
{
    struct SymMapKvp *kvp;

    if (sym_map->global == NULL) {
        return NULL;
    }

	if ((kvp = sym_map_find_shallow(sym_map, key))) {
		return kvp;
	}

    if (sym_map->global) {
		return sym_map_find_not_global(sym_map->global, key);
    } else {
        return NULL;
    }
}

void sym_map_for_each(struct SymMap *sym_map, void *state, void(*f)(void*, char*, VAL_LOC_T))
{
	struct SymMapKvp *kvp;

	if (sym_map->global) {
		sym_map_for_each(sym_map->global, state, f);
	}

	for (kvp = sym_map->map; kvp; kvp = kvp->next) {
		f(state, kvp->key, kvp->stack_loc);
	}
}

