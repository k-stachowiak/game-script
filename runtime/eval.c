/* Copyright (C) 2014-2016 Krzysztof Stachowiak */

#include "error.h"
#include "ast.h"
#include "symmap.h"
#include "rt_val.h"
#include "eval_detail.h"

void eval_error_not_found_src(
    char *symbol,
    struct SourceLocation *loc)
{
    err_push_src("EVAL", loc, "Symbol \"%s\" not found", symbol);
}

static void eval_symbol(
        struct AstNode *node,
        struct Runtime *rt,
        struct SymMap *sym_map,
	struct AstLocMap *alm)
{
    struct SymMapNode *smn;
    struct AstSymbol *symbol_node = &node->data.symbol;
    char *symbol = symbol_node->symbol;

    LOG_TRACE_FUNC;

    if (!(smn = sym_map_find(sym_map, symbol))) {
        eval_error_not_found_src(symbol, alm_get_ast(alm, node));
        return;
    }

    rt_val_push_copy(&rt->stack, smn->stack_loc);
}

VAL_LOC_T eval_dispatch(
        struct AstNode *node,
        struct Runtime *rt,
        struct SymMap *sym_map,
	struct AstLocMap *alm)
{
    /* It is possible that the debugger flag will change during evaluation. */
    bool debug_begin_called = false;

    VAL_LOC_T begin = rt->stack.top;

#if LOG_LEVEL <= LLVL_TRACE
    char *node_string = ast_serialize(node);
    LOG_TRACE("eval_impl BEGIN(%s)", node_string);
    mem_free(node_string);
#endif

    if (rt->debug) {
        dbg_call_begin(&rt->debugger, node);
        debug_begin_called = true;
    }

    switch (node->type) {
    case AST_SYMBOL:
        eval_symbol(node, rt, sym_map, alm);
        break;

    case AST_SPECIAL:
        eval_special(node, rt, sym_map, alm);
        break;

    case AST_FUNCTION_CALL:
        eval_func_call(node, rt, sym_map, alm);
        break;

    case AST_LITERAL_COMPOUND:
        eval_literal_compound(node, rt, sym_map, alm);
        break;

    case AST_LITERAL_ATOMIC:
        eval_literal_atomic(node, rt, sym_map, alm);
        break;
    }

    if (err_state()) {

        VAL_LOC_T ret_val = -1;
        if (debug_begin_called) {
            dbg_call_end(&rt->debugger, rt, ret_val, true);
        }

        err_push_src("EVAL", alm_get_ast(alm, node), "Failed evaluating expression");
	LOG_TRACE("eval_impl END(error)");
        return ret_val;

    } else {

        if (debug_begin_called) {
            dbg_call_end(&rt->debugger, rt, begin, false);
        }

	LOG_TRACE("eval_impl END(result=%td)", begin);
        return begin;
    }
}

VAL_LOC_T eval(
    struct AstNode *node,
    struct Runtime *rt,
    struct SymMap *sym_map,
    struct AstLocMap *alm)
{
    VAL_LOC_T begin, result, end;

#if LOG_LEVEL <= LLVL_TRACE
    char *node_string = ast_serialize(node);
    LOG_TRACE("eval BEGIN(%s)", node_string);
    mem_free(node_string);
#endif

    err_reset();

    /* The dispatch is delegated to another procedure as it is called
     * recursively during the evaluation while the current function is only
     * called by the top-level client.
     */
    begin = rt->stack.top;
    result = eval_dispatch(node, rt, sym_map, alm);
    end = rt->stack.top;

    if (err_state()) {
        stack_collapse(&rt->stack, begin, end);
        LOG_TRACE("eval END(error)");
        return -1;
    } else {
        LOG_TRACE("eval END(result=%td)", result);
        return result;
    }
}

VAL_LOC_T eval_bif(struct Runtime *rt, void *impl, VAL_SIZE_T arity)
{
    VAL_LOC_T size_loc, data_begin, result_loc = rt->stack.top;
    LOG_TRACE("eval_bif BEGIN");
    rt_val_push_func_init(&rt->stack, &size_loc, &data_begin, arity, VAL_FUNC_BIF, impl);
    rt_val_push_func_cap_init(&rt->stack, 0);
    rt_val_push_func_appl_init(&rt->stack, 0);
    rt_val_push_func_final(&rt->stack, size_loc, data_begin);
    LOG_TRACE("eval_bif END");
    return result_loc;
}

VAL_LOC_T eval_clif(struct Runtime *rt, void *impl, VAL_SIZE_T arity)
{
    VAL_LOC_T size_loc, data_begin, result_loc = rt->stack.top;
    LOG_TRACE("eval_clif END");
    rt_val_push_func_init(&rt->stack, &size_loc, &data_begin, arity, VAL_FUNC_CLIF, impl);
    rt_val_push_func_cap_init(&rt->stack, 0);
    rt_val_push_func_appl_init(&rt->stack, 0);
    rt_val_push_func_final(&rt->stack, size_loc, data_begin);
    LOG_TRACE("eval_clif END");
    return result_loc;
}

