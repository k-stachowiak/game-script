/* Copyright (C) 2014-2016 Krzysztof Stachowiak */

#ifndef AST_LOC_MAP_H
#define AST_LOC_MAP_H

#include "src_iter.h"

struct AstLocMapAstNode {
    struct AstNode *node;
    struct SourceLocation loc;
    struct AstLocMapAstNode *next;
};

struct AstLocMap {
    struct AstLocMapAstNode *root;
};

void alm_init(struct AstLocMap *alm);

void alm_deinit(struct AstLocMap *alm);

void alm_put(
    struct AstLocMap *alm,
    struct AstNode *node,
    struct SourceLocation *loc);

struct AstLocMapAstNode *alm_find(
    struct AstLocMap *alm,
    struct AstNode *node);

struct SourceLocation *alm_get(
    struct AstLocMap *alm,
    struct AstNode *node);

#endif
