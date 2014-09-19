/* Copyright (C) 2014 Krzysztof Stachowiak */

#include "ast.h"

struct AstNode *ast_parse_source(char *source);
struct AstNode *ast_parse_file(char *filename);
