/* Copyright (C) 2015 Krzysztof Stachowiak */

#include "ast.h"
#include "runtime.h"

struct Debugger {
	int lvl;
};

void dbg_init(struct Debugger *dbg);
void dbg_deinit(struct Debugger *dbg);
void dbg_callback_begin(void *dbg_void, struct AstNode* node);
void dbg_callback_end(void *dbg_void, struct AstNode* node);
