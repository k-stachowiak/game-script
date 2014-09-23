/* Copyright (C) 2014 Krzysztof Stachowiak */

#include "ast_parse.h"
#include "lex.h"
#include "parse.h"
#include "dom.h"
#include "error.h"

struct AstNode *ast_parse_source(char *source)
{
	struct DomNode *dom;
	struct AstNode *ast;

	dom = lex(source);
    if (err_state()) {
		return NULL;
	}

	ast = parse(dom);
    if (err_state()) {
		return NULL;
	}

	dom_free(dom);
	return ast;
}

struct AstNode *ast_parse_file(char *filename)
{
	char *source = my_getfile(filename);
	if (!source) {
        struct ErrMessage msg;
        err_msg_init(&msg, "PARSE");
        err_msg_append(&msg, "Failed loading file \"%s\"", filename);
        err_msg_set(&msg);
		return NULL;
	}

	return ast_parse_source(source);
}

