/* Copyright (C) 2015 Krzysztof Stachowiak */

#include <stdio.h>

#include "dbg.h"

static void dbg_print_node_bif(struct AstBif *bif)
{
}

static void dbg_print_node_do_block(struct AstDoBlock *do_block)
{
}

static void dbg_print_node_bind(struct AstBind *bind)
{
}

static void dbg_print_node_iff(struct AstIff *iff)
{
}

static void dbg_print_node_compound(struct AstCompound *compound)
{
}

static void dbg_print_node_func_call(struct AstFuncCall *func_call)
{
}

static void dbg_print_node_func_def(struct AstFuncDef *func_def)
{
}

static void dbg_print_node_literal(struct AstLiteral *literal)
{
}

static void dbg_print_node_reference(struct AstReference *reference)
{
}

static void dbg_print_node(struct AstNode *node)
{
	switch (node->type) {
    case AST_BIF:
		dbg_print_node_bif(&node->data.bif);
		break;

	case AST_DO_BLOCK:
		dbg_print_node_do_block(&node->data.do_block);
		break;

    case AST_BIND:
		dbg_print_node_bind(&node->data.bind);
		break;

	case AST_IFF:
		dbg_print_node_iff(&node->data.iff);
		break;

    case AST_COMPOUND:
		dbg_print_node_compound(&node->data.compound);
		break;

    case AST_FUNC_CALL:
		dbg_print_node_func_call(&node->data.func_call);
		break;

    case AST_FUNC_DEF:
		dbg_print_node_func_def(&node->data.func_def);
		break;

    case AST_LITERAL:
		dbg_print_node_literal(&node->data.literal);
		break;

	case AST_REFERENCE:
		dbg_print_node_reference(&node->data.reference);
		break;
	}
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

