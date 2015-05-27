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

void mn_set_debugger(bool state)
{
    runtime->debug = state;
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

	err_reset();

    if (!(source = my_getfile((char*)filename))) {
		err_push("LIB", "Failed loading a file");
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

	err_reset();

    expr = parse_source((char*)source);
    if (err_state()) {
		err_push("LIB", "Failed executing command: %s", source);
        return NULL;
    }

    rt_consume_one(runtime, expr, &result_loc, NULL);
    if (err_state()) {
		err_push("LIB", "Failed executing command: %s", source);
        return NULL;
    }

	if (result_loc) {
		return mn_make_api_value(result_loc);
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
