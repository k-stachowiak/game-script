/* Copyright (C) 2015 Krzysztof Stachowiak */

#include "stdlib.h"

#include "error.h"
#include "memory.h"
#include "collection.h"
#include "runtime.h"
#include "rt_val.h"
#include "parse.h"
#include "moon.h"

static struct Runtime *runtime;

static void mn_deinit(void)
{
	if (runtime) {
		rt_free(runtime);
	}
}

static struct MoonValue *make_api_value(VAL_LOC_T loc);

static struct MoonValue *make_api_value_compound(VAL_LOC_T loc)
{
	int i, len = rt_val_cpd_len(runtime, loc);
	VAL_LOC_T current_loc = rt_val_cpd_first_loc(loc);
	struct MoonValue *result = NULL, *result_end = NULL;
	for (i = 0; i < len; ++i) {
		struct MoonValue *value = make_api_value(current_loc);
		LIST_APPEND(value, &result, &result_end);
		current_loc = rt_val_next_loc(runtime, current_loc);
	}
	return result;
}

static struct MoonValue *make_api_value(VAL_LOC_T loc)
{
	int len;
	char *str;

	struct MoonValue *result = mem_malloc(sizeof(*result));
	result->next = NULL;

	switch (rt_val_peek_type(runtime, loc)) {
	case VAL_BOOL:
		result->type = MN_BOOL;
		result->data.boolean = rt_val_peek_bool(runtime, loc);
		break;

	case VAL_CHAR:
		result->type = MN_CHAR;
		result->data.character = rt_val_peek_char(runtime, loc);
		break;

	case VAL_INT:
		result->type = MN_INT;
		result->data.integer = rt_val_peek_int(runtime, loc);
		break;

	case VAL_REAL:
		result->type = MN_REAL;
		result->data.real = rt_val_peek_real(runtime, loc);
		break;

	case VAL_STRING:
		str = rt_val_peek_string(runtime, loc);
		len = strlen(str);
		result->type = MN_STRING;
		result->data.string = mem_malloc(len + 1);
		memcpy(result->data.string, str, len + 1);
		break;

	case VAL_ARRAY:
		result->type = MN_ARRAY;
		result->data.compound = make_api_value_compound(loc);
		break;

	case VAL_TUPLE:
		result->type = MN_TUPLE;
		result->data.compound = make_api_value_compound(loc);
		break;

	case VAL_FUNCTION:
		result->type = MN_FUNCTION;
		break;
	}

	return result;
}

static void api_value_free(struct MoonValue *value)
{
	while (value) {

		struct MoonValue *next_value = value->next;

		switch (value->type) {
		case MN_BOOL:
		case MN_CHAR:
		case MN_INT:
		case MN_REAL:
		case MN_FUNCTION:
			break;

		case MN_STRING:
			mem_free(value->data.string);
			break;

		case MN_ARRAY:
		case MN_TUPLE:
			api_value_free(value->data.compound);
			break;
		}

		free(value);
		value = next_value;
	}
}

void mn_init(void)
{
	mn_deinit();
	atexit(mn_deinit);
	runtime = rt_make(64 * 1024);
}

bool mn_exec_file(char *filename)
{
	struct AstNode *ast_list;

	if (!(ast_list = parse_file(filename))) {
		return false;
	}

	if (!rt_consume_list(runtime, ast_list)) {
		return false;
	}

	return true;
}

struct MoonValue *mn_exec(char *source)
{
	struct AstNode *expr;
	VAL_LOC_T result_loc;

	expr = parse_source(source);
	rt_consume_one(runtime, expr, &result_loc, NULL);

	if (err_state()) {
		return NULL;
	}

	return make_api_value(result_loc);
}

void mn_dispose(struct MoonValue* value)
{
	api_value_free(value);
}

char *mn_error_message(void)
{
	return err_msg();
}
