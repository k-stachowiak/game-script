/* Copyright (C) 2015 Krzysztof Stachowiak */

#include "error.h"
#include "eval_detail.h"

static void eval_bind_recursively(
		struct Runtime *rt,
		struct SymMap *sym_map,
		struct Pattern *pattern,
		VAL_LOC_T location,
		struct SourceLocation *source_loc)
{
	enum ValueType type = rt_val_peek_type(rt, location);

	struct Pattern *child_pat;
	VAL_LOC_T child_loc;

	if (pattern->type == PATTERN_SYMBOL) {
		sym_map_insert(sym_map, pattern->symbol, location, source_loc);
		return;
	}

	if (pattern->type == PATTERN_ARRAY && type != VAL_ARRAY) {
		err_incorrect_type();
		return;
	}

	if (pattern->type == PATTERN_TUPLE && type != VAL_TUPLE) {
		err_incorrect_type();
		return;
	}

	child_loc = rt_val_cpd_first_loc(location);
	child_pat = pattern->children;
	for (;;) {
	}
}

void eval_bind(
		struct AstNode *node,
		struct Runtime *rt,
		struct SymMap *sym_map)
{
	struct Pattern *pattern = node->data.bind.pattern;
	struct AstNode *expr = node->data.bind.expr;
    VAL_LOC_T location = eval_impl(expr, rt, sym_map);

    if (!err_state()) {
		eval_bind_recursively(rt, sym_map, pattern, location, &expr->loc);
    }
}

