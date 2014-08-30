/* Copyright (C) 2014 Krzysztof Stachowiak */

#include <stdlib.h>
#include <string.h>

#include "symmap.h"
#include "eval.h"
#include "bif.h"

static void sym_map_init_bifs(struct SymMap *sym_map, struct Stack *stack)
{
    bif_assure_init();
    sym_map_insert(sym_map, "sqrt", eval(&bif_sqrt, stack, sym_map));
    sym_map_insert(sym_map, "+", eval(&bif_add, stack, sym_map));
    sym_map_insert(sym_map, "-", eval(&bif_sub, stack, sym_map));
    sym_map_insert(sym_map, "*", eval(&bif_mul, stack, sym_map));
    sym_map_insert(sym_map, "/", eval(&bif_div, stack, sym_map));
}

void sym_map_init(
        struct SymMap *sym_map,
        struct SymMap *global,
        struct Stack *stack)
{
    sym_map->parent = global;
    sym_map->map = NULL;
    sym_map->end = NULL;

    if (global == NULL) {
        sym_map_init_bifs(sym_map, stack);
    }
}

void sym_map_deinit(struct SymMap *sym_map)
{
    struct SymMapKvp *temp, *kvp = sym_map->map;
    while (kvp) {
        free(kvp->key);
        temp = kvp;
        kvp = kvp->next;
        free(temp);
    }
    sym_map->map = NULL;
    sym_map->end = NULL;
}

void sym_map_insert(struct SymMap *sym_map, char *key, ptrdiff_t location)
{
    struct SymMapKvp *kvp;
    int len = strlen(key);
    char *key_copy = malloc(len + 1);
    memcpy(key_copy, key, len + 1);

    kvp = sym_map_find(sym_map, key);
    if (kvp) {
        kvp->location = location;
    }

    kvp = malloc(sizeof(*kvp));
    /* TODO: verify allocation result. */
    /* TODO: ensure sym map cleanup upon failures. */

    kvp->key = key_copy;
    kvp->location = location;
    kvp->next = NULL;

    LIST_APPEND(kvp, &sym_map->map, &sym_map->end);
}

struct SymMapKvp *sym_map_find(struct SymMap *sym_map, char *key)
{
    struct SymMapKvp *kvp = sym_map->map;
    while (kvp) {
        if (strcmp(kvp->key, key) == 0) {
            return kvp;
        }
        kvp = kvp->next;
    }

    if (sym_map->parent) {
        return sym_map_find(sym_map->parent, key);
    } else {
        return NULL;
    }
}

struct SymMapKvp *sym_map_find_not_global(struct SymMap *sym_map, char *key)
{
    struct SymMapKvp *kvp;

    if (sym_map->parent == NULL) {
        return NULL;
    }

    kvp = sym_map->map;
    while (kvp) {
        if (strcmp(kvp->key, key) == 0) {
            return kvp;
        }
        kvp = kvp->next;
    }

    if (sym_map->parent) {
        return sym_map_find(sym_map->parent, key);
    } else {
        return NULL;
    }
}
