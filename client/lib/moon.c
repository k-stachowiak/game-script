/* Copyright (C) 2015 Krzysztof Stachowiak */

#include "stdlib.h"

#include "error.h"
#include "memory.h"
#include "term.h"
#include "collection.h"
#include "runtime.h"
#include "rt_val.h"
#include "parse.h"
#include "mndetail.h"
#include "clif.h"

static void moon_error_nofile(void)
{
    struct ErrMessage msg;
    err_msg_init(&msg, "LIB");
    err_msg_append(&msg, "Failed loading a file");
    err_msg_set(&msg);
}

static void mn_deinit(void)
{
	clif_deinit();
    if (runtime) {
        rt_free(runtime);
    }
}

void mn_init(void)
{
    mn_deinit();
    atexit(mn_deinit);
    runtime = rt_make();
	clif_init();
}

void mn_register_clif(const char *symbol, int arity, ClifHandler handler)
{
	clif_register((char*)symbol, handler);
	rt_register_clif_handler(runtime, (char*)symbol, arity, clif_common_handler);
}

bool mn_exec_file(const char *filename)
{
    char *source;
    struct AstNode *ast_list;

    if (!(source = my_getfile((char*)filename))) {
        moon_error_nofile();
        return false;
    }

    if (!(ast_list = parse_source(source))) {
        mem_free(source);
        return false;
    }

    if (!rt_consume_list(runtime, ast_list, NULL)) {
        mem_free(source);
        return false;
    }

    mem_free(source);
    return true;
}

struct MoonValue *mn_exec_command(const char *source)
{
    struct AstNode *expr;
    VAL_LOC_T result_loc;

    expr = parse_source((char*)source);
    rt_consume_one(runtime, expr, &result_loc, NULL);

    if (err_state()) {
        return NULL;
    }

    return mn_make_api_value(result_loc);
}

void mn_dispose(struct MoonValue* value)
{
    mn_api_value_free(value);
}

const char *mn_error_message(void)
{
    return err_msg();
}
