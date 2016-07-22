/* Copyright (C) 2014-2016 Krzysztof Stachowiak */

#include "stdlib.h"

#include "error.h"
#include "memory.h"
#include "term.h"
#include "collection.h"
#include "runtime.h"
#include "rt_val.h"
#include "parse.h"
#include "api_value.h"

struct MoonContext {
    struct Runtime *rt;
};

struct MoonContext *mn_create(void)
{
    struct MoonContext *result = mem_malloc(sizeof(*result));
    atexit(err_reset);
    result->rt = rt_make();
    return result;
}

void mn_destroy(struct MoonContext *ctx)
{
    rt_free(ctx->rt);
    mem_free(ctx);
}

void mn_set_debugger(struct MoonContext *ctx, bool state)
{
    ctx->rt->debug = state;
}

bool mn_register_clif(struct MoonContext *ctx, const char *symbol, int arity, ClifHandler handler)
{
    rt_register_clif_handler(ctx->rt, (char*)symbol, arity, handler);
    return !err_state();
}

bool mn_exec_file(struct MoonContext *ctx, const char *filename)
{
    char *source;
    struct AstNode *ast_list;
    struct AstLocMap alm;

    err_reset();

    if (!(source = my_getfile((char*)filename))) {
        err_push("LIB", "Failed loading a file");
        return false;
    }

    if (!(ast_list = parse_source_build_alm(source, &alm))) {
        mem_free(source);
        return false;
    }

    if (!rt_consume_list(ctx->rt, ast_list, &alm, NULL)) {
        mem_free(source);
        return false;
    }

    mem_free(source);
    return true;
}

struct MoonValue *mn_exec_command(struct MoonContext *ctx, const char *source)
{
    struct AstNode *expr;
    VAL_LOC_T result_loc;
    struct AstLocMap alm;

    err_reset();
    alm_init(&alm);

    expr = parse_source_build_alm((char*)source, &alm);
    if (err_state()) {
        err_push("LIB", "Failed executing command: %s", source);
	alm_deinit(&alm);
        return NULL;
    }

    rt_consume_one(ctx->rt, expr, &alm, &result_loc, NULL);
    if (err_state()) {
        err_push("LIB", "Failed executing command: %s", source);
	alm_deinit(&alm);
        return NULL;
    }

    alm_deinit(&alm);

    if (result_loc) {
        return mn_make_api_value(ctx->rt, result_loc);
    } else {
        return NULL;
    }
}

void mn_dispose(struct MoonValue* value)
{
    mn_api_value_free(value);
}

bool mn_error_state(void)
{
    return err_state();
}

const char *mn_error_message(void)
{
    return err_msg();
}

