/* Copyright (C) 2014-2016 Krzysztof Stachowiak */

#ifndef PARSE_H
#define PARSE_H

#include "ast.h"

struct AstNode;
struct Pattern;
struct AstLocMap;

typedef void (*ParserAstCallback)(void*, struct AstNode*, struct SourceLocation*);

/**
 * Parses source code to produce an abstract syntax tree.
 */
struct AstNode *parse_source(
    char *source,
    void *data,
    ParserAstCallback acb);

struct AstNode *parse_source_build_alm(
    char *source,
    struct AstLocMap *alm);

#endif
