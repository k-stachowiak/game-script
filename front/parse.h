/* Copyright (C) 2014,2015 Krzysztof Stachowiak */

#ifndef PARSE_H
#define PARSE_H

#include "ast.h"

/**
 * Parses source code to produce an abstract syntax tree.
 */
struct AstNode *parse_source(char *source);

#endif
