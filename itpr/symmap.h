/* Copyright (C) 2014 Krzysztof Stachowiak */

#include <stddef.h>

#include "stack.h"
#include "common.h"

#ifndef SYMMAP_H
#define SYMMAP_H

struct SymMap {
    struct SymMap *global;
    struct SymMapKvp *map;
    struct SymMapKvp *end;
};

struct SymMapKvp {
    char *key;
	VAL_LOC_T stack_loc;
    struct SourceLocation source_loc;
    struct SymMapKvp *next;
};

void sym_map_init_global(
		struct SymMap *sym_map,
		struct Stack *stack);

void sym_map_init_local(
        struct SymMap *sym_map,
        struct SymMap *global);

void sym_map_deinit(struct SymMap *sym_map);

void sym_map_insert(
        struct SymMap *sym_map,
        char *key,
        VAL_LOC_T stack_loc,
        struct SourceLocation *source_loc);

struct SymMapKvp *sym_map_find(struct SymMap *sym_map, char *key);
struct SymMapKvp *sym_map_find_shallow(struct SymMap *sym_map, char *key);
struct SymMapKvp *sym_map_find_not_global(struct SymMap *sym_map, char *key);
void sym_map_for_each(struct SymMap *sym_map, void(*f)(char*, VAL_LOC_T));

#endif
