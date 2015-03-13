/* Copyright (C) 2015 Krzysztof Stachowiak */

#include <stdio.h>

#include "dbg.h"

static void dbg_print_node(struct AstNode *node)
{
}

void dbg_init(struct Debugger *dbg)
{
	dbg->lvl = 0;
}

void dbg_deinit(struct Debugger *dbg)
{
}

void dbg_callback_begin(void *dbg_void, struct AstNode* node)
{
	int i;
	struct Debugger *dbg = (struct Debugger*)dbg_void;
	for (i = 0; i < dbg->lvl * 2; ++i) {
		putc(' ', stdout);
	}
	dbg_print_node(node);
	++dbg->lvl;
}

void dbg_callback_end(void *dbg_void, struct AstNode* node)
{
	struct Debugger *dbg = (struct Debugger*)dbg_void;
	--dbg->lvl;
}

