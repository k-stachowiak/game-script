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

struct MoonContext {
	struct Runtime *rt;
	struct ClifMap *cm;
};

struct MoonContext *mn_init(void)
{
	struct MoonContext *result = mem_malloc(sizeof(*result));
	result->rt = rt_make();
	result->cm = clif_make();
	return result;
}

void mn_deinit(struct MoonContext *mc)
{
	clif_free(mc->cm);
	rt_free(mc->rt);
	mem_free(mc);
}

void mn_set_debugger(struct MoonContext *mc, bool state)
{
    mc->rt->debug = state;
}

void mn_register_clif(struct MoonContext *mc, const char *symbol, int arity, ClifHandler handler)
{
	clif_register(mc->cm, (char*)symbol, handler);
	rt_register_clif_handler(mc->rt, (char*)symbol, arity, clif_common_handler);
}

bool mn_exec_file(struct MoonContext *mc, const char *filename)
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

    if (!rt_consume_list(mc->rt, ast_list, NULL)) {
        mem_free(source);
        return false;
    }

    mem_free(source);
    return true;
}

struct MoonValue *mn_exec_command(struct MoonContext *mc, const char *source)
{
    struct AstNode *expr;
    VAL_LOC_T result_loc;

	err_reset();

    expr = parse_source((char*)source);
    if (err_state()) {
		err_push("LIB", "Failed executing command: %s", source);
        return NULL;
    }

    rt_consume_one(mc->rt, expr, &result_loc, NULL);
    if (err_state()) {
		err_push("LIB", "Failed executing command: %s", source);
        return NULL;
    }

	if (result_loc) {
		return mn_make_api_value(mc->rt, result_loc);
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
