/* Copyright (C) 2014 Krzysztof Stachowiak */

#include "ast_parse.h"
#include "lex.h"
#include "parse.h"
#include "dom.h"

struct AstNode *ast_parse_source(char *source)
{
	struct DomNode *dom;
	struct AstNode *ast;

	if (!(dom = lex(source))) {
		return NULL;
	}

	if (!(ast = parse(dom))) {
		dom_free(dom);
		return NULL;
	}

	dom_free(dom);

	return ast;
}

struct AstNode *ast_parse_file(char *filename)
{
	char *source = my_getfile(filename);
	if (!source) {
		printf("Failed loading file \"%s\".\n", filename);
		return NULL;
	}

	return ast_parse_source(source);
}

