#ifndef PARSE_H
#define PARSE_H

#include "ast.h"
#include "dom.h"

struct AstNode *parse(struct DomNode *dom);

#endif
