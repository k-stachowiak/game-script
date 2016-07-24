/* Copyright (C) 2014-2016 Krzysztof Stachowiak */

#include <stdlib.h>
#include "memory.h"
#include "ast_loc_map.h"

void alm_init(struct AstLocMap *alm)
{
    alm->ast_root = NULL;
    alm->pat_root = NULL;
}

void alm_deinit(struct AstLocMap *alm)
{
    while (alm->ast_root) {
    struct AstLocMapAstNode *next = alm->ast_root->next;
    free(alm->ast_root);
    alm->ast_root = next;
    }
    while (alm->pat_root) {
    struct AstLocMapPatNode *next = alm->pat_root->next;
    free(alm->pat_root);
    alm->pat_root = next;
    }
}

void alm_put_ast(
    struct AstLocMap *alm,
    struct AstNode *node,
    struct SourceLocation *loc)
{
    struct AstLocMapAstNode *map_node = mem_malloc(sizeof(*map_node));
    map_node->node = node;
    map_node->loc = *loc;
    map_node->next = alm->ast_root;
    alm->ast_root = map_node;
}

void alm_put_pat(
    struct AstLocMap *alm,
    struct Pattern *pattern,
    struct SourceLocation *loc)
{
    struct AstLocMapPatNode *map_node = mem_malloc(sizeof(*map_node));
    map_node->pattern = pattern;
    map_node->loc = *loc;
    map_node->next = alm->pat_root;
    alm->pat_root = map_node;
}

struct AstLocMapAstNode *alm_find_ast(
    struct AstLocMap *alm,
    struct AstNode *node)
{
    struct AstLocMapAstNode *map_node = alm->ast_root;
    while (map_node && map_node->node != node) {
    map_node = map_node->next;
    }
    return map_node;
}

struct SourceLocation *alm_get_ast(
    struct AstLocMap *alm,
    struct AstNode *node)
{
    struct AstLocMapAstNode *found = alm_find_ast(alm, node);
    if (!found) {
    return NULL;
    } else {
    return &found->loc;
    }
}

struct AstLocMapPatNode *alm_find_pat(
    struct AstLocMap *alm,
    struct Pattern *pattern)
{
    struct AstLocMapPatNode *map_node = alm->pat_root;
    while (map_node && map_node->pattern != pattern) {
    map_node = map_node->next;
    }
    return map_node;
}

struct SourceLocation *alm_get_pat(
    struct AstLocMap *alm,
    struct Pattern *pattern)
{
    struct AstLocMapPatNode *found = alm_find_pat(alm, pattern);
    if (!found) {
    return NULL;
    } else {
    return &found->loc;
    }
}
