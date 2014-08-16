/* Copyright (C) 2014 Krzysztof Stachowiak */

#ifndef BIF_H
#define BIF_H

#include "ast.h"

extern struct AstNode bif_sqrt;

extern struct AstNode bif_add;
extern struct AstNode bif_sub;
extern struct AstNode bif_mul;
extern struct AstNode bif_div;

void bif_assure_init(void);

#endif
