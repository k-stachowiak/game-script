/* Copyright (C) 2015 Krzysztof Stachowiak */

#include "collection.h"
#include "error.h"
#include "mndetail.h"
#include "rt_val.h"
#include "runtime.h"
#include "clif.h"

static struct MoonValue *clif_read_args(struct Runtime *rt, VAL_LOC_T *arg_locs, int arg_count)
{
	if (arg_count == 0) {
		return NULL;
	}

	struct MoonValue *result = mn_make_api_value(rt, *arg_locs);
	result->next = clif_read_args(rt, arg_locs + 1, arg_count - 1);
	return result;
}

static void clif_push_result(struct Runtime* rt, struct MoonValue *value)
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
			clif_push_result(rt, child);
			child = child->next;
		}
		data_end = rt->stack.top;
		rt_val_push_cpd_final(&rt->stack, size_loc, data_end - data_begin);
		break;

    case MN_TUPLE:
		rt_val_push_tuple_init(&rt->stack, &size_loc);
		data_begin = rt->stack.top;
		while (child) {
			clif_push_result(rt, child);
			child = child->next;
		}
		data_end = rt->stack.top;
		rt_val_push_cpd_final(&rt->stack, size_loc, data_end - data_begin);
		break;

	case MN_FUNCTION:
		LOG_ERROR("Function manipulation not yet handled in the library.");
		exit(1);
	}
}

struct ClifMap *clif_make(void)
{
	struct ClifMap *result = mem_malloc(sizeof(*result));
	result->map = NULL;
	result->end = NULL;
	return result;
}

void clif_free(struct ClifMap *clif_map)
{
	struct ClifMapKvp *temp, *kvp = clif_map->map;
	while (kvp) {
		temp = kvp;
		kvp = kvp->next;
		mem_free(temp->key);
		mem_free(temp);
	}
	clif_map->map = NULL;
	clif_map->end = NULL;
	mem_free(clif_map);
}

void clif_register(struct ClifMap *clif_map, char *symbol, ClifHandler handler)
{
    struct ClifMapKvp *kvp = clif_map->map;
    int len = strlen(symbol);
    char *symbol_copy = mem_malloc(len + 1);
    memcpy(symbol_copy, symbol, len + 1);

    while (kvp) {
		if (strcmp(kvp->key, symbol) == 0) {
			err_push("LIB", "Symbol \"%s\" already inserted", symbol);
			return;
		}
		kvp = kvp->next;
    }

    kvp = mem_malloc(sizeof(*kvp));
    kvp->key = symbol_copy;
    kvp->handler = handler;
    kvp->next = NULL;

    LIST_APPEND(kvp, &clif_map->map, &clif_map->end);
}

void clif_common_handler(struct Runtime *rt, char *symbol, VAL_LOC_T *arg_locs, int arg_count)
{
	struct ClifMapKvp *kvp = clif_map.map;

	while (kvp) {
		struct MoonValue *arg_list;
		struct MoonValue *result;

		if (strcmp(kvp->key, symbol) != 0) {
			kvp = kvp->next;
			continue;
		}

		arg_list = clif_read_args(rt, arg_locs, arg_count);
		result = kvp->handler(arg_list);
		mn_dispose(arg_list);

		if (result) {
			clif_push_result(rt, result);
			mn_dispose(result);
		}
		return;
	}

	err_push("LIB", "Symbol \"%s\" not found", symbol);
}

