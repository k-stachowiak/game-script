/* Copyright (C) 2014,2015 Krzysztof Stachowiak */

#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "collection.h"
#include "error.h"
#include "memory.h"
#include "symmap.h"
#include "eval.h"

void sym_map_init_global(struct SymMap *sym_map)
{
    sym_map->global = NULL;
    memset(&sym_map->root, 0, sizeof(sym_map->root));
}

void sym_map_init_local(
        struct SymMap *sym_map,
        struct SymMap *global)
{
    sym_map->global = global;
    memset(&sym_map->root, 0, sizeof(sym_map->root));
}

static void sym_map_node_free(struct SymMapNode *node)
{
    int i;
    for (i = 0; i < node->children.size; ++i) {
        sym_map_node_free(node->children.data + i);
    }
    mem_free(node->children.data);
}

void sym_map_deinit(struct SymMap *sym_map)
{
    sym_map_node_free(&sym_map->root);
}

void sym_map_insert(
        struct SymMap *sym_map,
        char *key,
        VAL_LOC_T stack_loc,
        struct SourceLocation source_loc)
{
    struct SymMapNode *node = &sym_map->root;
    char *current = key;
    while (*current) {
        int i;
        bool found = false;
        for (i = 0; i < node->children.size; ++i) {
            if (node->children.data[i].key == *current) {
                node = node->children.data + i;
                found = true;
                break;
            }
        }
        if (!found) {
            struct SymMapNode new_node = {
                0,
                { NULL, 0, 0 },
                false,
                0,
                { 0, 0, 0 }
            };
            new_node.key = *current;
            ARRAY_APPEND(node->children, new_node);
            node = node->children.data + i;
        }
        ++current;
    }
    if (node->is_set) {
        err_push("RUNTIME", "Symbol \"%s\" already inserted", key);
    } else {
        node->is_set = true;
        node->stack_loc = stack_loc;
        node->source_loc = source_loc;
    }
}

struct SymMapNode *sym_map_find(struct SymMap *sym_map, char *key)
{
    struct SymMapNode *node;

    if ((node = sym_map_find_shallow(sym_map, key))) {
        return node;
    }

    if (sym_map->global) {
        return sym_map_find(sym_map->global, key);
    } else {
        return NULL;
    }
}

struct SymMapNode *sym_map_find_shallow(struct SymMap *sym_map, char *key)
{
    struct SymMapNode *node = &sym_map->root;
    while (*key) {
        int i;
        bool found = false;
        for (i = 0; i < node->children.size; ++i) {
            if (node->children.data[i].key == *key) {
                node = node->children.data + i;
                ++key;
                found = true;
                break;
            }
        }
        if (!found) {
            return NULL;
        }
    }
    if (node->is_set) {
        return node;
    } else {
        return NULL;
    }
}

struct SymMapNode *sym_map_find_not_global(struct SymMap *sym_map, char *key)
{
    struct SymMapNode *node;

    if (sym_map->global == NULL) {
        return NULL;
    }

    if ((node = sym_map_find_shallow(sym_map, key))) {
        return node;
    }

    if (sym_map->global) {
        return sym_map_find_not_global(sym_map->global, key);
    } else {
        return NULL;
    }
}

