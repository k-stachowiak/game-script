/* Copyright (C) 2015 Krzysztof Stachowiak */

#include "collection.h"
#include "error.h"
#include "mndetail.h"
#include "rt_val.h"

struct ClifMapKvp {
	char *key;
	ClifHandler handler;
	struct ClifMapKvp *next;
};

struct ClifMap {
	struct ClifMapKvp *map;
	struct ClifMapKvp *end;
} clif_map;

static struct MoonValue *clif_read_args(VAL_LOC_T *arg_locs, int arg_count)
{
	if (arg_count == 0) {
		return NULL;
	}

	struct MoonValue *result = mn_make_api_value(*arg_locs);
	result->next = clif_read_args(arg_locs + 1, arg_count - 1);
	return result;
}

static void clif_push_result(struct MoonValue *value)
{
	VAL_LOC_T size_loc, data_begin, data_end;
	struct MoonValue *child = value->data.compound;

	switch (value->type) {
    case MN_BOOL:
		rt_val_push_bool(runtime->stack, value->data.boolean);
		break;

    case MN_CHAR:
		rt_val_push_char(runtime->stack, value->data.character);
		break;

    case MN_INT:
		rt_val_push_int(runtime->stack, value->data.integer);
		break;

    case MN_REAL:
		rt_val_push_real(runtime->stack, value->data.real);
		break;

    case MN_STRING:
		rt_val_push_string(
			runtime->stack,
			value->data.string,
			value->data.string + strlen(value->data.string));
		break;

    case MN_ARRAY:
		rt_val_push_array_init(runtime->stack, &size_loc);
		data_begin = runtime->stack->top;
		while (child) {
			clif_push_result(child);
			child = child->next;
		}
		data_end = runtime->stack->top;
		rt_val_push_cpd_final(runtime->stack, size_loc, data_end - data_begin);
		break;

    case MN_TUPLE:
		rt_val_push_tuple_init(runtime->stack, &size_loc);
		data_begin = runtime->stack->top;
		while (child) {
			clif_push_result(child);
			child = child->next;
		}
		data_end = runtime->stack->top;
		rt_val_push_cpd_final(runtime->stack, size_loc, data_end - data_begin);
		break;

	case MN_FUNCTION:
		LOG_ERROR("Function manipulation not yet handled in the library.");
		exit(1);
	}
}

void clif_init(void)
{
	clif_map.map = NULL;
	clif_map.end = NULL;
}

void clif_deinit(void)
{
	struct ClifMapKvp *temp, *kvp = clif_map.map;
	while (kvp) {
		temp = kvp;
		kvp = kvp->next;
		mem_free(temp->key);
		mem_free(temp);
	}
	clif_map.map = NULL;
	clif_map.end = NULL;
}

void clif_register(char *symbol, ClifHandler handler)
{
    struct ClifMapKvp *kvp = clif_map.map;
    int len = strlen(symbol);
    char *symbol_copy = mem_malloc(len + 1);
    memcpy(symbol_copy, symbol, len + 1);

    while (kvp) {
		if (strcmp(kvp->key, symbol) == 0) {
			err_push_virt("LIB", "Symbol \"%s\" already inserted", symbol);
			return;
		}
		kvp = kvp->next;
    }

    kvp = mem_malloc(sizeof(*kvp));
    kvp->key = symbol_copy;
    kvp->handler = handler;
    kvp->next = NULL;

    LIST_APPEND(kvp, &clif_map.map, &clif_map.end);
}

void clif_common_handler(char *symbol, VAL_LOC_T *arg_locs, int arg_count)
{
	struct ClifMapKvp *kvp = clif_map.map;

	while (kvp) {
		struct MoonValue *arg_list;
		struct MoonValue *result;

		if (strcmp(kvp->key, symbol) != 0) {
			kvp = kvp->next;
			continue;
		}

		arg_list = clif_read_args(arg_locs, arg_count);
		result = kvp->handler(arg_list);

		clif_push_result(result);

		mn_dispose(arg_list);
		mn_dispose(result);

		return;
	}

	err_push_virt("LIB", "Symbol \"%s\" not found", symbol);
}

