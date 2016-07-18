/* Copyright (C) 2014 Krzysztof Stachowiak */

#ifndef LEX_H
#define LEX_H

#include "tok.h"
#include "dom.h"

/**
 * Performs lexical analysis of the source string and returns
 * the document object model of the extended symbolic expressions.
 */
struct DomNode *lex(char *source);

#endif
