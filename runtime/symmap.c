/* Copyright (C) 2014,2015 Krzysztof Stachowiak */

#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "collection.h"
#include "error.h"
#include "memory.h"
#include "symmap.h"
#include "eval.h"

struct CharArray { char *data; int size, cap; };
struct SerializationState { char* string; };

void sym_map_init_global(struct SymMap *sym_map)
{
    sym_map->parent = NULL;
    memset(&sym_map->root, 0, sizeof(sym_map->root));
}

void sym_map_init_local(
        struct SymMap *sym_map,
        struct SymMap *parent)
{
    sym_map->parent = parent;
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
    LOG_TRACE(
        "sym_map_insert(%s, %td, {%d, %d})",
        key, stack_loc, source_loc.line, source_loc.column);
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
        char *string = sym_map_serialize(sym_map);
        LOG_ERROR("Symbol map at error point:\n%s", string);
        mem_free(string);
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

    if (sym_map->parent) {
        return sym_map_find(sym_map->parent, key);
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

    if (sym_map->parent == NULL) {
        return NULL;
    }

    if ((node = sym_map_find_shallow(sym_map, key))) {
        return node;
    }

    if (sym_map->parent) {
        return sym_map_find_not_global(sym_map->parent, key);
    } else {
        return NULL;
    }
}

static void sym_map_for_each_rec(
        struct SymMapNode *node,
        void (*callback)(char*, struct SymMapNode*, void*),
        void *data,
        struct CharArray *current_symbol)
{
    /* 0. Overview:
     * This function traverses the symbol map trie recursively
     * calling the callback function for every occurence of a value attached
     * in a given point in the trie.
     */

    int i, count;
    struct SymMapNode *child;

    /* 1. Check for the callback condition */
    if (node->is_set) {
        callback(current_symbol->data, node, data);
    }

    /* 2. Traverse the deeper nodes recursively */
    count = node->children.size;
    child = node->children.data;
    for (i = 0; i < count; ++i) {

        /* 2.1. Append the current character */
        ARRAY_POP(*current_symbol);
        ARRAY_APPEND(*current_symbol, child->key);
        ARRAY_APPEND(*current_symbol, '\0');

        /* 2.2. Make the recursive call */
        sym_map_for_each_rec(child, callback, data, current_symbol);

        /* 2.3. Remove the last appended character */
        ARRAY_POP(*current_symbol);
        ARRAY_POP(*current_symbol);
        ARRAY_APPEND(*current_symbol, '\0');

        /* 2.4. Advance the current child pointer */
        ++child;
    }
}

void sym_map_for_each(
        struct SymMap *sym_map,
        void (*callback)(char*, struct SymMapNode*, void*),
        void *data)
{
    struct CharArray current_symbol = { NULL, 0, 0 };
    ARRAY_APPEND(current_symbol, '\0');
    sym_map_for_each_rec(&sym_map->root, callback, data, &current_symbol);
    ARRAY_FREE(current_symbol);
}

static void sym_map_serialize_callback(
        char *symbol,
        struct SymMapNode *node,
        void *data)
{
    struct SerializationState *state = (struct SerializationState *)data;
    str_append(
        state->string,
        "%s -> %td @ %d,%d\n",
        symbol,
        node->stack_loc,
        node->source_loc.line,
        node->source_loc.column);
}

char *sym_map_serialize(struct SymMap *sym_map)
{
    struct SerializationState state = { NULL };
    while (sym_map) {
        sym_map_for_each(sym_map, sym_map_serialize_callback, &state);
        str_append(state.string, "> next parent\n");
        sym_map = sym_map->parent;
    }
    return state.string;
}
