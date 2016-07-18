/* Copyright (C) 2014 Krzysztof Stachowiak */

#include <stddef.h>

#include "runtime.h"

#ifndef SYMMAP_H
#define SYMMAP_H

struct SymMapNode {
    char key;
    struct { struct SymMapNode *data; int cap, size; } children;
    bool is_set;
    VAL_LOC_T stack_loc;
    struct SourceLocation source_loc;
};

struct SymMap {
    struct SymMap *parent;
    struct SymMapNode root;
};

void sym_map_init_global(struct SymMap *sym_map);
void sym_map_init_local(struct SymMap *sym_map, struct SymMap *parent);
void sym_map_deinit(struct SymMap *sym_map);

void sym_map_insert(
        struct SymMap *sym_map,
        char *key,
        VAL_LOC_T stack_loc,
        struct SourceLocation source_loc);

struct SymMapNode *sym_map_find(struct SymMap *sym_map, char *key);
struct SymMapNode *sym_map_find_shallow(struct SymMap *sym_map, char *key);
struct SymMapNode *sym_map_find_not_global(struct SymMap *sym_map, char *key);

void sym_map_for_each(
        struct SymMap *sym_map,
        void (*callback)(char*, struct SymMapNode*, void*),
        void *data);

char *sym_map_serialize(struct SymMap *sym_map);

#endif
