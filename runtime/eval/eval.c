/* Copyright (C) 2014,2015 Krzysztof Stachowiak */

#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

#include "log.h"
#include "collection.h"
#include "eval.h"
#include "eval_detail.h"
#include "bif.h"
#include "error.h"
#include "runtime.h"
#include "rt_val.h"

void eval_error_not_found(char *symbol)
{
    err_push("EVAL", "Symbol \"%s\" not found", symbol);
}

static void eval_literal(struct AstNode *node, struct Stack *stack)
{
    char *string;
    int string_len;

    switch (node->data.literal.type) {
    case AST_LIT_UNIT:
        rt_val_push_unit(stack);
        break;

    case AST_LIT_BOOL:
        rt_val_push_bool(stack, node->data.literal.data.boolean);
        break;

    case AST_LIT_CHAR:
        rt_val_push_char(stack, node->data.literal.data.character);
        break;

    case AST_LIT_INT:
        rt_val_push_int(stack, node->data.literal.data.integer);
        break;

    case AST_LIT_REAL:
        rt_val_push_real(stack, node->data.literal.data.real);
        break;

    case AST_LIT_STRING:
        string = node->data.literal.data.string;
        string_len = strlen(string);
        rt_val_push_string(stack, string + 1, string + string_len - 1);
        break;
    }
}

static void eval_do_block(
        struct AstNode *node,
        struct Runtime *rt,
        struct SymMap *sym_map)
{
    struct SymMap local_sym_map;
    struct AstNode *expr = node->data.do_block.exprs;

    sym_map_init_local(&local_sym_map, sym_map);

    VAL_LOC_T begin = rt->stack.top;
    VAL_LOC_T end = rt->stack.top;

    for (; expr; expr = expr->next) {
        VAL_LOC_T new_end = eval_impl(expr, rt, &local_sym_map);
        if (err_state()) {
            err_push_src("EVAL", node->loc, "Failed evaluating do expression");
            break;
        } else {
            end = new_end;
        }
    }

    stack_collapse(&rt->stack, begin, end);

    sym_map_deinit(&local_sym_map);
}

static void eval_reference(
        struct AstNode *node,
        struct Stack *stack,
        struct SymMap *sym_map)
{
    struct SymMapKvp *kvp;
    char *symbol = node->data.reference.symbol;

    LOG_TRACE_FUNC;

    if (!(kvp = sym_map_find(sym_map, symbol))) {
        eval_error_not_found(symbol);
        return;
    }

    rt_val_push_copy(stack, kvp->stack_loc);
}

/* Main evaluation dispatch.
 * =========================
 */

VAL_LOC_T eval_impl(
        struct AstNode *node,
        struct Runtime *rt,
        struct SymMap *sym_map)
{
    /* It is possible that the debugger flag will change during evaluation. */
    bool debug_begin_called = false;
    VAL_LOC_T begin = rt->stack.top;
    if (rt->debug) {
        dbg_call_begin(&rt->debugger, node);
        debug_begin_called = true;
    }

    switch (node->type) {
    case AST_COMPOUND:
        eval_compound(node, rt, sym_map);
        break;

    case AST_LITERAL:
        eval_literal(node, &rt->stack);
        break;

    case AST_DO_BLOCK:
        eval_do_block(node, rt, sym_map);
        break;

    case AST_BIND:
        eval_bind(node, rt, sym_map);
        break;

    case AST_REFERENCE:
        eval_reference(node, &rt->stack, sym_map);
        break;

    case AST_FUNC_DEF:
        eval_func_def(node, &rt->stack, sym_map);
        break;

    case AST_PARAFUNC:
        eval_parafunc(node, rt, sym_map);
        break;

    case AST_FUNC_CALL:
        eval_func_call(node, rt, sym_map);
        break;

    default:
        LOG_ERROR("Unhandled AST node type.\n");
        exit(1);
    }

    if (err_state()) {

        VAL_LOC_T ret_val = -1;
        if (debug_begin_called) {
            dbg_call_end(&rt->debugger, rt, ret_val, true);
        }

        err_push_src("EVAL", node->loc, "Failed evaluating expression");
        return ret_val;

    } else {

        if (debug_begin_called) {
            dbg_call_end(&rt->debugger, rt, begin, false);
        }

        return begin;
    }
}

VAL_LOC_T eval(struct AstNode *node, struct Runtime *rt, struct SymMap *sym_map)
{
    VAL_LOC_T begin, result, end;

    err_reset();

    begin = rt->stack.top;
    result = eval_impl(node, rt, sym_map);
    end = rt->stack.top;

    if (err_state()) {
        err_push_src("EVAL", node->loc, "Failed evaluating expression");
        stack_collapse(&rt->stack, begin, end);
        return -1;
    } else {
        return result;
    }
}

VAL_LOC_T eval_bif(struct Runtime *rt, void *impl, VAL_SIZE_T arity)
{
    VAL_LOC_T size_loc, data_begin, result_loc = rt->stack.top;
    rt_val_push_func_init(&rt->stack, &size_loc, &data_begin, arity, VAL_FUNC_BIF, impl);
    rt_val_push_func_cap_init(&rt->stack, 0);
    rt_val_push_func_appl_init(&rt->stack, 0);
    rt_val_push_func_final(&rt->stack, size_loc, data_begin);
    return result_loc;
}

VAL_LOC_T eval_clif(struct Runtime *rt, void *impl, VAL_SIZE_T arity)
{
    VAL_LOC_T size_loc, data_begin, result_loc = rt->stack.top;
    rt_val_push_func_init(&rt->stack, &size_loc, &data_begin, arity, VAL_FUNC_CLIF, impl);
    rt_val_push_func_cap_init(&rt->stack, 0);
    rt_val_push_func_appl_init(&rt->stack, 0);
    rt_val_push_func_final(&rt->stack, size_loc, data_begin);
    return result_loc;
}

