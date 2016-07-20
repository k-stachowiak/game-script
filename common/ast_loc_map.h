/* Copyright (C) 2014-2016 Krzysztof Stachowiak */

#ifndef AST_LOC_MAP_H
#define AST_LOC_MAP_H

#include "src_iter.h"

struct AstLocMapAstNode {
    struct AstNode *node;
    struct SourceLocation loc;
    struct AstLocMapAstNode *next;
};

struct AstLocMapPatNode {
    struct Pattern *pattern;
    struct SourceLocation loc;
    struct AstLocMapPatNode *next;
};

struct AstLocMap {
    struct AstLocMapAstNode *ast_root;
    struct AstLocMapPatNode *pat_root;
};

void alm_init(struct AstLocMap *alm);

void alm_deinit(struct AstLocMap *alm);

void alm_put_ast(
    struct AstLocMap *alm,
    struct AstNode *node,
    struct SourceLocation *loc);

void alm_put_pat(
    struct AstLocMap *alm,
    struct Pattern *pattern,
    struct SourceLocation *loc);

struct AstLocMapAstNode *alm_find_ast(
    struct AstLocMap *alm,
    struct AstNode *node);

struct SourceLocation *alm_get_ast(
    struct AstLocMap *alm,
    struct AstNode *node);

struct AstLocMapPatNode *alm_find_pat(
    struct AstLocMap *alm,
    struct Pattern *pattern);

struct SourceLocation *alm_get_pat(
    struct AstLocMap *alm,
    struct Pattern *pattern);

#endif
