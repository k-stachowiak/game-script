/* Copyright (C) 2014 Krzysztof Stachowiak */

#ifndef BIF_H
#define BIF_H

#include "ast.h"

#define BIF_MAX_ARITY 3

extern struct AstNode bif_sqrt;

extern struct SourceLocation bif_location;

extern struct AstNode bif_add;
extern struct AstNode bif_sub;
extern struct AstNode bif_mul;
extern struct AstNode bif_div;
extern struct AstNode bif_mod;

extern struct AstNode bif_eq;
extern struct AstNode bif_lt;
extern struct AstNode bif_gt;
extern struct AstNode bif_leq;
extern struct AstNode bif_geq;

extern struct AstNode bif_and;
extern struct AstNode bif_or;
extern struct AstNode bif_not;

extern struct AstNode bif_length;
extern struct AstNode bif_empty;
extern struct AstNode bif_car;
extern struct AstNode bif_cdr;
extern struct AstNode bif_reverse;
extern struct AstNode bif_cons;
extern struct AstNode bif_cat;
extern struct AstNode bif_slice;

void bif_assure_init(void);

#endif