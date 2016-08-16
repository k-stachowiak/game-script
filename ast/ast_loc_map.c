/* Copyright (C) 2014-2016 Krzysztof Stachowiak */

#include <stdlib.h>
#include "memory.h"
#include "ast_loc_map.h"

void alm_init(struct AstLocMap *alm)
{
    alm->root = NULL;
}

void alm_deinit(struct AstLocMap *alm)
{
    while (alm->root) {
        struct AstLocMapAstNode *next = alm->root->next;
        free(alm->root);
        alm->root = next;
    }
}

void alm_put(
        struct AstLocMap *alm,
        struct AstNode *node,
        struct SourceLocation *loc)
{
    struct AstLocMapAstNode *map_node = mem_malloc(sizeof(*map_node));
    map_node->node = node;
    map_node->loc = *loc;
    map_node->next = alm->root;
    alm->root = map_node;
}

struct AstLocMapAstNode *alm_find(
        struct AstLocMap *alm,
        struct AstNode *node)
{
    struct AstLocMapAstNode *map_node = alm->root;
    while (map_node && map_node->node != node) {
        map_node = map_node->next;
    }
    return map_node;
}

struct SourceLocation *alm_get(
        struct AstLocMap *alm,
        struct AstNode *node)
{
    struct AstLocMapAstNode *found = alm_find(alm, node);
    if (!found) {
        return NULL;
    } else {
        return &found->loc;
    }
}

void alm_try_put(
    struct AstLocMap *alm,
    struct AstNode *node,
    struct SourceLocation *loc)
{
    if (alm) {
        alm_put(alm, node, loc);
    }
}

struct AstLocMapAstNode *alm_try_find(
    struct AstLocMap *alm,
    struct AstNode *node)
{
    if (alm) {
        return alm_find(alm, node);
    } else {
        return NULL;
    }
}

struct SourceLocation *alm_try_get(
    struct AstLocMap *alm,
    struct AstNode *node)
{
    if (alm) {
        return alm_get(alm, node);
    } else {
        return NULL;
    }
}
