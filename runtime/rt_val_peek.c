/* Copyright (C) 2015 Krzysztof Stachowiak */

#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

#include "strbuild.h"
#include "rt_val.h"
#include "log.h"

struct ValueHeader rt_val_peek_header(struct Stack *stack, VAL_LOC_T location)
{
	struct ValueHeader result;
	char *src = stack->buffer + location;
	memcpy(&result.type, src, VAL_HEAD_TYPE_BYTES);
	memcpy(&result.size, src + VAL_HEAD_TYPE_BYTES, VAL_HEAD_SIZE_BYTES);
	return result;
}

static void rt_val_to_string_compound(struct Runtime *rt, VAL_LOC_T x, char **str)
{
	int i, len = rt_val_cpd_len(rt, x);
	VAL_LOC_T item = rt_val_cpd_first_loc(x);
	for (i = 0; i < len; ++i) {
		rt_val_to_string(rt, item, str);
		str_append(*str, " ");
		item = rt_val_next_loc(rt, item);
	}
}

void rt_val_to_string(struct Runtime *rt, VAL_LOC_T x, char **str)
{
	enum ValueType type = rt_val_peek_type(rt, x);

	if (rt_val_is_string(rt, x)) {
		char *string = rt_val_peek_cpd_as_string(rt, x);
		str_append(*str, "%s", string);
		mem_free(string);
		return;
	}

	switch (type) {
	case VAL_BOOL:
		if (rt_val_peek_bool(rt, x)) {
			str_append(*str, "true");
		}
		else {
			str_append(*str, "false");
		}
		break;

	case VAL_CHAR:
		str_append(*str, "%c", rt_val_peek_char(rt, x));
		break;

	case VAL_INT:
		str_append(*str, "%" PRIu64, rt_val_peek_int(rt, x));
		break;

	case VAL_REAL:
		str_append(*str, "%f", rt_val_peek_real(rt, x));
		break;

	case VAL_ARRAY:
		str_append(*str, "[ ");
		rt_val_to_string_compound(rt, x, str);
		str_append(*str, "]");
		break;

	case VAL_TUPLE:
		str_append(*str, "{ ");
		rt_val_to_string_compound(rt, x, str);
		str_append(*str, "}");
		break;

	case VAL_FUNCTION:
		str_append(*str, "function");
		break;
	}
}

void rt_val_print(struct Runtime *rt, VAL_LOC_T loc, bool annotate)
{
	char *buffer = NULL;

	if (annotate) {
		if (rt_val_is_string(rt, loc)) {
			str_append(buffer, "string :: ");
		}
		else {
			switch (rt_val_peek_type(rt, loc)) {
			case VAL_BOOL:
				str_append(buffer, "bool :: ");
				break;

			case VAL_CHAR:
				str_append(buffer, "char :: ");
				break;

			case VAL_INT:
				str_append(buffer, "integer :: ");
				break;

			case VAL_REAL:
				str_append(buffer, "real :: ");
				break;

			case VAL_ARRAY:
				str_append(buffer, "array :: ");
				break;

			case VAL_TUPLE:
				str_append(buffer, "tuple :: ");
				break;

			case VAL_FUNCTION:
				str_append(buffer, "function :: ");
				break;
			}
		}
	}

	rt_val_to_string(rt, loc, &buffer);
	printf("%s", buffer);
	free(buffer);
}

bool rt_val_is_string(struct Runtime *rt, VAL_LOC_T loc)
{
	if (rt_val_peek_type(rt, loc) != VAL_ARRAY) {
		return false;
	}

	if (rt_val_cpd_len(rt, loc) == 0) {
		return true;
	}

	loc = rt_val_cpd_first_loc(loc);
	if (rt_val_peek_type(rt, loc) != VAL_CHAR) {
		return false;
	}

	return true;
}

int rt_val_cpd_len(struct Runtime *rt, VAL_LOC_T location)
{
	VAL_LOC_T current, end;
	int len = 0;

	current = location + VAL_HEAD_BYTES;
	end = current + rt_val_peek_size(rt, location);

	while (current != end) {
		current = rt_val_next_loc(rt, current);
		++len;
	}

	return len;
}

VAL_LOC_T rt_val_next_loc(struct Runtime *rt, VAL_LOC_T loc)
{
	struct ValueHeader header = rt_val_peek_header(rt->stack, loc);
	return loc + VAL_HEAD_BYTES + header.size;
}

enum ValueType rt_val_peek_type(struct Runtime *rt, VAL_LOC_T loc)
{
	struct ValueHeader header = rt_val_peek_header(rt->stack, loc);
	return (enum ValueType)header.type;
}

VAL_SIZE_T rt_val_peek_size(struct Runtime *rt, VAL_LOC_T loc)
{
	struct ValueHeader header = rt_val_peek_header(rt->stack, loc);
	return header.size;
}

VAL_BOOL_T rt_val_peek_bool(struct Runtime *rt, VAL_LOC_T loc)
{
	VAL_BOOL_T result;
	memcpy(&result, rt->stack->buffer + loc + VAL_HEAD_BYTES, VAL_BOOL_BYTES);
	return result;
}

VAL_CHAR_T rt_val_peek_char(struct Runtime *rt, VAL_LOC_T loc)
{
	VAL_CHAR_T result;
	memcpy(&result, rt->stack->buffer + loc + VAL_HEAD_BYTES, VAL_CHAR_BYTES);
	return result;
}

VAL_INT_T rt_val_peek_int(struct Runtime *rt, VAL_LOC_T loc)
{
	VAL_INT_T result;
	memcpy(&result, rt->stack->buffer + loc + VAL_HEAD_BYTES, VAL_INT_BYTES);
	return result;
}

VAL_REAL_T rt_val_peek_real(struct Runtime *rt, VAL_LOC_T loc)
{
	VAL_REAL_T result;
	memcpy(&result, rt->stack->buffer + loc + VAL_HEAD_BYTES, VAL_REAL_BYTES);
	return result;
}

VAL_LOC_T rt_val_cpd_first_loc(VAL_LOC_T loc)
{
	return loc + VAL_HEAD_BYTES;
}

char* rt_val_peek_cpd_as_string(struct Runtime *rt, VAL_LOC_T loc)
{
	VAL_LOC_T current = rt_val_cpd_first_loc(loc);
	int i, len = rt_val_cpd_len(rt, loc);
	char *result = mem_malloc(len + 1);
	for (i = 0; i < len; ++i) {
		result[i] = rt_val_peek_char(rt, current);
		current = rt_val_next_loc(rt, current);
	}
	result[i] = '\0';
	return result;
}

struct ValueFuncData rt_val_function_data(struct Runtime *rt, VAL_LOC_T loc)
{
	int cap_count, i;
	struct ValueFuncData result;
	VAL_LOC_T cap_count_loc, appl_count_loc;

	/* Read locations.
	* ===============
	*/

	loc += VAL_HEAD_BYTES;

	result.arity_loc = loc;
	loc += VAL_SIZE_BYTES;

	result.type_loc = loc;
	loc += VAL_TYPE_BYTES;

	result.impl_loc = loc;
	loc += VAL_PTR_BYTES;

	cap_count_loc = loc;
	result.cap_start = cap_count_loc + VAL_SIZE_BYTES;

	cap_count = stack_peek_size(rt->stack, loc);

	loc += VAL_SIZE_BYTES;

	for (i = 0; i < cap_count; ++i) {
		loc = rt_val_fun_next_cap_loc(rt, loc);
	}

	appl_count_loc = loc;
	result.appl_start = appl_count_loc + VAL_SIZE_BYTES;

	/* Lookup values.
	* ==============
	*/

	result.arity = stack_peek_size(rt->stack, result.arity_loc);
	result.func_type = stack_peek_type(rt->stack, result.type_loc);
	result.impl = (void*)stack_peek_ptr(rt->stack, result.impl_loc);
	result.cap_count = stack_peek_size(rt->stack, cap_count_loc);
	result.appl_count = stack_peek_size(rt->stack, appl_count_loc);

	return result;
}

char *rt_val_peek_fun_cap_symbol(struct Runtime *rt, VAL_LOC_T cap_loc)
{
	return rt->stack->buffer + VAL_SIZE_BYTES + cap_loc;
}

VAL_LOC_T rt_val_fun_cap_loc(struct Runtime *rt, VAL_LOC_T cap_loc)
{
	VAL_SIZE_T len;
	memcpy(&len, rt->stack->buffer + cap_loc, VAL_SIZE_BYTES);
	return cap_loc + VAL_SIZE_BYTES + len;
}

VAL_LOC_T rt_val_fun_next_cap_loc(struct Runtime *rt, VAL_LOC_T loc)
{
	return rt_val_next_loc(rt, rt_val_fun_cap_loc(rt, loc));
}

VAL_LOC_T rt_val_fun_next_appl_loc(struct Runtime *rt, VAL_LOC_T loc)
{
	return rt_val_next_loc(rt, loc);
}