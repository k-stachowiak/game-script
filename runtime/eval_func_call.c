/* Copyright (C) 2014-2016 Krzysztof Stachowiak */

#include "ast.h"
#include "bif.h"
#include "error.h"
#include "collection.h"
#include "rt_val.h"
#include "symmap.h"
#include "eval_detail.h"
#include "api_value.h"

struct LocArray { VAL_LOC_T *data; int size, cap; };

static void efc_get_already_applied_locs(
        struct Runtime *rt,
        struct ValueFuncData *func_data,
        struct LocArray *result)
{
    VAL_SIZE_T i;
    VAL_LOC_T appl_loc = func_data->appl_start;
    for (i = 0; i < func_data->appl_count; ++i) {
        ARRAY_APPEND(*result, appl_loc);
        appl_loc = rt_val_next_loc(rt, appl_loc);
    }
}

static bool efc_evaluate_currently_applied(
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct AstNode *args,
        struct LocArray *result,
    struct AstLocMap *alm)
{
    while (args) {
        VAL_LOC_T loc = eval_dispatch(args, rt, sym_map, alm);
        if (err_state()) {
            err_push("EVAL", "Failed evaluating new funtcion arguments");
            return false;
        }
        ARRAY_APPEND(*result, loc);
        args = args->next;
    }
    return true;
}

/** Evaluates an expression and inserts into a provided symbol map. */
static bool efc_evaluate_arg(
        struct Runtime *rt,
        struct SymMap *caller_sym_map,
        struct SymMap *new_sym_map,
        struct AstNode *arg_node,
        struct AstNode *pattern,
        VAL_LOC_T *location,
        struct AstLocMap *alm)
{
    *location = eval_dispatch(arg_node, rt, caller_sym_map, alm);
    if (err_state()) {
        err_push_src(
            "EVAL",
            alm_get(alm, arg_node),
            "Failed evaluating function argument expression");
        return false;
    }

    eval_special_bind_pattern(pattern, *location, rt, new_sym_map, alm);
    if (err_state()) {
        err_push_src(
            "EVAL",
            alm_get(alm, pattern),
            "Failed registering function argument in the local scope");
        return false;
    }

    return true;
}

/**
 * Creates a new function value based on and existing function value,
 * Its captures, already evaluated arguments and the newly applied ones.
 */
static void efc_curry_on(
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct AstNode *actual_args,
        struct ValueFuncData *func_data,
    struct AstLocMap *alm)
{
    VAL_LOC_T current_loc, size_loc, data_begin;
    VAL_SIZE_T i, arg_count;

    /* Initialize push */
    rt_val_push_func_init(
        &rt->stack,
        &size_loc,
        &data_begin,
        func_data->arity,
        func_data->func_type,
        func_data->impl);

    /* Captures */
    rt_val_push_func_cap_init(&rt->stack, func_data->cap_count);
    current_loc = func_data->cap_start;
    for (i = 0; i < func_data->cap_count; ++i) {
        rt_val_push_func_cap_copy(&rt->stack, current_loc);
        current_loc += rt_val_fun_next_cap_loc(rt, current_loc);
    }

    /* Applied... */
    arg_count = func_data->appl_count + ast_list_len(actual_args);

    /* ...already, ... */
    rt_val_push_func_appl_init(&rt->stack, arg_count);
    current_loc = func_data->appl_start;
    for (i = 0; i < func_data->appl_count; ++i) {
        rt_val_push_copy(&rt->stack, current_loc);
        current_loc += rt_val_fun_next_appl_loc(rt, current_loc);
    }

    /* ...currently. */
    for (; actual_args; actual_args = actual_args->next) {
        eval_dispatch(actual_args, rt, sym_map, alm);
        if (err_state()) {
            err_push_src(
                "EVAL",
                alm_get(alm, actual_args),
                "Failed evaluating function argument");
            break;
        }
    }

    /* Finalize push */
    rt_val_push_func_final(&rt->stack, size_loc, data_begin);
}

/** Evaluates a general function implementation i.e. not BIF. */
static void efc_evaluate_ast(
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct AstNode *actual_args,
        struct ValueFuncData *func_data,
    struct AstLocMap *alm)
{
    VAL_SIZE_T i;

    VAL_LOC_T temp_begin, temp_end;
    VAL_LOC_T cap_loc = func_data->cap_start;
    VAL_LOC_T appl_loc = func_data->appl_start;

    struct SymMap captures_sym_map;
    struct SymMap args_sym_map;

    struct AstNode *node = (struct AstNode *)func_data->impl;
    struct AstSpecFuncDef *fdef = &node->data.special.data.func_def;

    struct AstNode *formal_args = fdef->formal_args;

    /* Initialize local scopes hierarchy. */
    sym_map_init_local(&captures_sym_map, sym_map);
    sym_map_init_local(&args_sym_map, &captures_sym_map);

    /* Insert captures into the scope. */
    LOG_TRACE("Evaluate AST call: captures scope");
    for (i = 0; i < func_data->cap_count; ++i) {
        char *cap_symbol = rt_val_peek_fun_cap_symbol(rt, cap_loc);
        VAL_LOC_T cap_val_loc = rt_val_fun_cap_loc(rt, cap_loc);
        sym_map_insert(&captures_sym_map, cap_symbol, cap_val_loc);
        cap_loc = rt_val_fun_next_cap_loc(rt, cap_loc);
        if (err_state()) {
            err_push("EVAL", "Failed re-evaluating funtcion captures");
            goto cleanup;
        }
    }

    /* Insert already applied arguments. */
    LOG_TRACE("Evaluate AST call: already applied in args scope");
    for (i = 0; i < func_data->appl_count; ++i) {
        eval_special_bind_pattern(formal_args, appl_loc, rt, &args_sym_map, alm);
        formal_args = formal_args->next;
        appl_loc = rt_val_fun_next_appl_loc(rt, appl_loc);
        if (err_state()) {
            err_push("EVAL", "Failed re-evaluating funtcion applied arguments");
            goto cleanup;
        }
    }

    /* Evaluate and insert new arguments. */
    LOG_TRACE("Evaluate AST call: applied now in args scope");
    temp_begin = rt->stack.top;
    for (; actual_args; actual_args = actual_args->next) {
        VAL_LOC_T actual_loc;
        if (efc_evaluate_arg(
        rt, sym_map, &args_sym_map,
        actual_args, formal_args,
        &actual_loc, alm)) {
            formal_args = formal_args->next;
        } else {
            goto cleanup;
        }
    }
    temp_end = rt->stack.top;

    /* Evaluate the function expression. */
    eval_dispatch(fdef->expr, rt, &args_sym_map, alm);

    /* Collapse the temporaries. */
    stack_collapse(&rt->stack, temp_begin, temp_end);

cleanup:

    /* Free the local scope. */
    sym_map_deinit(&args_sym_map);
    sym_map_deinit(&captures_sym_map);
}

/** Evaluates a BIF. */
static void efc_evaluate_bif(
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct AstNode *actual_args,
        struct ValueFuncData *func_data,
    struct AstLocMap *alm)
{
    struct LocArray arg_locs = { NULL, 0, 0 };
    VAL_LOC_T temp_begin, temp_end;

    efc_get_already_applied_locs(rt, func_data, &arg_locs);

    temp_begin = rt->stack.top;
    if (!efc_evaluate_currently_applied(rt, sym_map, actual_args, &arg_locs, alm)) {
        goto cleanup;
    }
    temp_end = rt->stack.top;

    if (arg_locs.size > BIF_MAX_ARITY) {
        LOG_ERROR("Argument count mismatch.\n");
        exit(1);
    }
    if (func_data->arity != arg_locs.size) {
        LOG_ERROR("Invalid argument count passed to BIF.");
        exit(1);
    }

    /* Evaluate the function implementation. */
    switch (func_data->arity) {
    case 1:
        ((bif_unary_func)func_data->impl)(rt, arg_locs.data[0]);
        break;

    case 2:
        ((bif_binary_func)func_data->impl)(rt, arg_locs.data[0], arg_locs.data[1]);
        break;

    case 3:
        ((bif_ternary_func)func_data->impl)(rt, arg_locs.data[0], arg_locs.data[1], arg_locs.data[2]);
        break;
    }

    /* Collapse the temporaries. */
    stack_collapse(&rt->stack, temp_begin, temp_end);

cleanup:
    ARRAY_FREE(arg_locs);
}

static struct MoonValue *efc_eval_client_args(struct Runtime *rt, VAL_LOC_T *arg_locs, int arg_count)
{
    if (arg_count == 0) {
        return NULL;
    }

    struct MoonValue *result = mn_make_api_value(rt, *arg_locs);
    result->next = efc_eval_client_args(rt, arg_locs + 1, arg_count - 1);
    return result;
}

static void efc_push_client_result(struct Runtime *rt, struct MoonValue *value)
{
    VAL_LOC_T size_loc, data_begin, data_end;
    struct MoonValue *child = value->data.compound;

    switch (value->type) {
    case MN_BOOL:
        rt_val_push_bool(&rt->stack, value->data.boolean);
        break;

    case MN_CHAR:
        rt_val_push_char(&rt->stack, value->data.character);
        break;

    case MN_INT:
        rt_val_push_int(&rt->stack, value->data.integer);
        break;

    case MN_REAL:
        rt_val_push_real(&rt->stack, value->data.real);
        break;

    case MN_STRING:
        rt_val_push_string(
            &rt->stack,
            value->data.string,
            value->data.string + strlen(value->data.string));
        break;

    case MN_ARRAY:
        rt_val_push_array_init(&rt->stack, &size_loc);
        data_begin = rt->stack.top;
        while (child) {
            efc_push_client_result(rt, child);
            child = child->next;
        }
        data_end = rt->stack.top;
        rt_val_push_cpd_final(&rt->stack, size_loc, data_end - data_begin);
        break;

    case MN_TUPLE:
        rt_val_push_tuple_init(&rt->stack, &size_loc);
        data_begin = rt->stack.top;
        while (child) {
            efc_push_client_result(rt, child);
            child = child->next;
        }
        data_end = rt->stack.top;
        rt_val_push_cpd_final(&rt->stack, size_loc, data_end - data_begin);
        break;

    case MN_UNIT:
        rt_val_push_unit(&rt->stack);
        break;

    case MN_FUNCTION:
        err_push("EVAL", "CLIF returned a function");
        break;

    case MN_REFERENCE:
        err_push("EVAL", "CLIF returned a pointer");
        break;
    }
}

/** Evaluates a CLIF. */
static void efc_evaluate_clif(
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct AstNode *actual_args,
        struct ValueFuncData *func_data,
    struct AstLocMap *alm)
{
    struct LocArray arg_locs = { NULL, 0, 0 };
    VAL_LOC_T temp_begin, temp_end;
    struct MoonValue *client_args, *client_result;
    ClifHandler handler = (ClifHandler)func_data->impl;

    efc_get_already_applied_locs(rt, func_data, &arg_locs);

    temp_begin = rt->stack.top;
    if (!efc_evaluate_currently_applied(rt, sym_map, actual_args, &arg_locs, alm)) {
        goto cleanup;
    }
    temp_end = rt->stack.top;

    client_args = efc_eval_client_args(rt, arg_locs.data, arg_locs.size);
    client_result = handler(client_args);
    mn_api_value_free(client_args);

    if (client_result) {
        efc_push_client_result(rt, client_result);
        mn_api_value_free(client_result);
    }

    stack_collapse(&rt->stack, temp_begin, temp_end);

cleanup:
    ARRAY_FREE(arg_locs);
}

void eval_func_call(
        struct AstNode *node,
        struct Runtime *rt,
        struct SymMap *sym_map,
    struct AstLocMap *alm)
{
    VAL_LOC_T temp_begin, temp_end;
    struct AstFuncCall *fcall = &node->data.func_call;
    struct AstNode *func = fcall->func;
    struct AstNode *actual_args = fcall->actual_args;

    VAL_LOC_T func_loc;
    struct ValueFuncData func_data;
    VAL_SIZE_T applied;

    temp_begin = rt->stack.top;
    func_loc = eval_dispatch(func, rt, sym_map, alm);
    temp_end = rt->stack.top;
    if (err_state()) {
        err_push_src(
            "EVAL",
            alm_get(alm, func),
            "Failed evaluating function identity");
        return;
    }

    if (rt_val_peek_type(&rt->stack, func_loc) != VAL_FUNCTION) {
        err_push_src(
            "EVAL",
            alm_get(alm, func),
            "Function call key doesn't evaluate to a function");
        return;
    }

    func_data = rt_val_function_data(rt, func_loc);
    applied = func_data.appl_count + ast_list_len(actual_args);

    if (func_data.arity > applied) {
        efc_curry_on(rt, sym_map, actual_args, &func_data, alm);

    } else if (func_data.arity == applied) {
        switch (func_data.func_type) {
        case VAL_FUNC_AST:
            efc_evaluate_ast(rt, sym_map, actual_args, &func_data, alm);
            break;

        case VAL_FUNC_BIF:
            efc_evaluate_bif(rt, sym_map, actual_args, &func_data, alm);
            break;

        case VAL_FUNC_CLIF:
            efc_evaluate_clif(rt, sym_map, actual_args, &func_data, alm);
            break;
        }

    } else {
        err_push("EVAL", "Passed too many arguments to a function");

    }

    stack_collapse(&rt->stack, temp_begin, temp_end);
}

