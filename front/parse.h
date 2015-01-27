/* Copyright (C) 2014,2015 Krzysztof Stachowiak */

#ifndef PARSE_H
#define PARSE_H

#include "ast.h"

struct AstNode *parse_source(char *source);
struct AstNode *parse_file(char *filename);

#endif
