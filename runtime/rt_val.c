/* Copyright (C) 2014,2015 Krzysztof Stachowiak */

#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "log.h"
#include "runtime.h"
#include "rt_val.h"

static VAL_HEAD_SIZE_T zero = 0;

static void rt_val_print_compound(
		struct Runtime *rt, VAL_LOC_T loc, char open, char close)
{
	VAL_LOC_T cpd_loc, end;

	printf("%c ", open);

	cpd_loc = rt_val_cpd_first_loc(loc);
	end = cpd_loc + rt_val_peek_size(rt, loc);
	while (cpd_loc != end) {
		rt_val_print(rt, cpd_loc, false);
		printf(" ");
		cpd_loc = rt_val_next_loc(rt, cpd_loc);
	}

	printf("%c", close);
}

static void rt_val_print_function(struct Runtime *rt, VAL_LOC_T loc)
{
	VAL_LOC_T impl_loc, cap_start, appl_start;
	VAL_SIZE_T captures, applied, arity;
	struct AstNode *def;

	rt_val_function_locs(rt, loc, &impl_loc, &cap_start, &appl_start);

	def = (struct AstNode*)stack_peek_ptr(rt->stack, impl_loc);

	if (def->type == AST_BIF) {
		printf("built-in function");
		return;
	}

	if (def->type != AST_FUNC_DEF) {
		LOG_ERROR("Attempting to print non-func value with func printing.\n");
		exit(1);
	}

	captures = rt_val_peek_size(rt, cap_start);
	applied = rt_val_peek_size(rt, appl_start);
	arity = def->data.func_def.func.arg_count - applied;
	printf("function (ar=%d, cap=%d, appl=%d)", arity, captures, applied);
}

/* Writing / pushing.
 * ==================
 */

void rt_val_push_bool(struct Stack *stack, VAL_BOOL_T value)
{
	VAL_HEAD_TYPE_T type = (VAL_HEAD_TYPE_T)VAL_BOOL;
	VAL_HEAD_SIZE_T size = VAL_BOOL_BYTES;
	stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type);
	stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&size);
	stack_push(stack, size, (char*)&value);
}

void rt_val_push_char(struct Stack *stack, VAL_CHAR_T value)
{
	VAL_HEAD_TYPE_T type = (VAL_HEAD_TYPE_T)VAL_CHAR;
	VAL_HEAD_SIZE_T size = VAL_CHAR_BYTES;
	stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type);
	stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&size);
	stack_push(stack, size, (char*)&value);
}

void rt_val_push_int(struct Stack *stack, VAL_INT_T value)
{
	VAL_HEAD_TYPE_T type = (VAL_HEAD_TYPE_T)VAL_INT;
	VAL_HEAD_SIZE_T size = VAL_INT_BYTES;
	stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type);
	stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&size);
	stack_push(stack, size, (char*)&value);
}

void rt_val_push_real(struct Stack *stack, VAL_REAL_T value)
{
	VAL_HEAD_TYPE_T type = (VAL_HEAD_TYPE_T)VAL_REAL;
	VAL_HEAD_SIZE_T size = VAL_REAL_BYTES;
	stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type);
	stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&size);
	stack_push(stack, size, (char*)&value);
}

void rt_val_push_string(struct Stack *stack, char *value)
{
	VAL_HEAD_TYPE_T type = (VAL_HEAD_TYPE_T)VAL_STRING;
	VAL_HEAD_SIZE_T size = strlen(value) + 1;
	stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type);
	stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&size);
	stack_push(stack, size, value);
}

void rt_val_push_array_init(struct Stack *stack, VAL_LOC_T *size_loc)
{
	VAL_HEAD_TYPE_T type = (VAL_HEAD_TYPE_T)VAL_ARRAY;
	stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type);
	*size_loc = stack_push(stack, VAL_SIZE_BYTES, (char*)&zero);
}

void rt_val_push_tuple_init(struct Stack *stack, VAL_LOC_T *size_loc)
{
	VAL_HEAD_TYPE_T type = (VAL_HEAD_TYPE_T)VAL_TUPLE;
	stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type);
	*size_loc = stack_push(stack, VAL_SIZE_BYTES, (char*)&zero);
}

void rt_val_push_cpd_final(
        struct Stack *stack,
        VAL_LOC_T size_loc,
        VAL_SIZE_T size)
{
	memcpy(stack->buffer + size_loc, &size, VAL_HEAD_SIZE_BYTES);
}

void rt_val_push_func_init(
        struct Stack *stack,
        VAL_LOC_T *size_loc,
        VAL_LOC_T *data_begin,
        void *func_def)
{
	static VAL_HEAD_TYPE_T type = (VAL_HEAD_TYPE_T)VAL_FUNCTION;
	stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type);
	*size_loc = stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&zero);
	*data_begin = stack_push(stack, VAL_PTR_BYTES, (char*)&func_def);
}

void rt_val_push_func_cap_init_deferred(
        struct Stack *stack,
        VAL_LOC_T *cap_count_loc)
{
	*cap_count_loc = stack_push(stack, VAL_SIZE_BYTES, (char*)&zero);
}

void rt_val_push_func_cap_init(struct Stack *stack, VAL_SIZE_T cap_count)
{
	stack_push(stack, VAL_SIZE_BYTES, (char*)&cap_count);
}

void rt_val_push_func_cap(struct Stack *stack, char *symbol, VAL_LOC_T loc)
{
	VAL_SIZE_T len = strlen(symbol) + 1;
	stack_push(stack, VAL_SIZE_BYTES, (char*)&len);
	stack_push(stack, len, symbol);
	stack_push_copy(stack, loc);
}

void rt_val_push_func_cap_copy(struct Stack *stack, VAL_LOC_T loc)
{
    char *symbol = stack->buffer + loc;
    VAL_SIZE_T len = strlen(symbol);
	stack_push(stack, VAL_SIZE_BYTES, (char*)&len);
	stack_push(stack, len + 1, symbol);
	stack_push_copy(stack, loc + len + 1);
}

void rt_val_push_func_cap_final_deferred(
        struct Stack *stack,
        VAL_LOC_T cap_count_loc,
        VAL_SIZE_T cap_count)
{
	memcpy(stack->buffer + cap_count_loc, &cap_count, VAL_SIZE_BYTES);
}

void rt_val_push_func_appl_init(struct Stack *stack, VAL_SIZE_T appl_count)
{
	stack_push(stack, VAL_SIZE_BYTES, (char*)&appl_count);
}

void rt_val_push_func_appl_empty(struct Stack *stack)
{
    stack_push(stack, VAL_SIZE_BYTES, (char*)&zero);
}

void rt_val_push_func_final(
        struct Stack *stack,
        VAL_LOC_T size_loc,
        VAL_SIZE_T data_begin)
{
	VAL_LOC_T data_size = stack->top - data_begin;
	memcpy(stack->buffer + size_loc, &data_size, VAL_HEAD_SIZE_BYTES);
}

/* Reading / peeking.
 * ==================
 */

struct ValueHeader rt_val_peek_header(struct Stack *stack, VAL_LOC_T location)
{
	struct ValueHeader result;
	char *src = stack->buffer + location;
	memcpy(&result.type, src, VAL_HEAD_TYPE_BYTES);
	memcpy(&result.size, src + VAL_HEAD_TYPE_BYTES, VAL_HEAD_SIZE_BYTES);
	return result;
}

void rt_val_print(struct Runtime *rt, VAL_LOC_T loc, bool annotate)
{
	switch (rt_val_peek_type(rt, loc)) {
	case VAL_BOOL:
		if (annotate) {
			printf("bool :: ");
		}
		printf("%s", rt_val_peek_bool(rt, loc) ? "true" : "false");
		break;

	case VAL_CHAR:
		if (annotate) {
			printf("char :: ");
		}
		printf("'%c'", rt_val_peek_char(rt, loc));
		break;

	case VAL_INT:
		if (annotate) {
			printf("integer :: ");
		}
		printf("%" PRId64, rt_val_peek_int(rt, loc));
		break;

	case VAL_REAL:
		if (annotate) {
			printf("real :: ");
		}
		printf("%f", rt_val_peek_real(rt, loc));
		break;

	case VAL_STRING:
		if (annotate) {
			printf("string :: ");
		}
		printf("%s", rt_val_peek_string(rt, loc));
		break;

	case VAL_ARRAY:
		if (annotate) {
			printf("array :: ");
		}
		rt_val_print_compound(rt, loc, '[', ']');
		break;

	case VAL_TUPLE:
		if (annotate) {
			printf("tuple :: ");
		}
		rt_val_print_compound(rt, loc, '{', '}');
		break;

	case VAL_FUNCTION:
		rt_val_print_function(rt, loc);
		break;
	}
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

char* rt_val_peek_string(struct Runtime *rt, VAL_LOC_T loc)
{
    return rt->stack->buffer + loc + VAL_HEAD_BYTES;
}

VAL_LOC_T rt_val_cpd_first_loc(VAL_LOC_T loc)
{
    return loc + VAL_HEAD_BYTES;
}

void rt_val_function_locs(
        struct Runtime *rt, 
        VAL_LOC_T loc,
        VAL_LOC_T *impl_loc,
        VAL_LOC_T *cap_start,
        VAL_LOC_T *appl_start)
{
    int cap_count, i;

	loc += VAL_HEAD_BYTES;

    *impl_loc = loc;
    loc += VAL_PTR_BYTES;

    *cap_start = loc;
    cap_count = stack_peek_size(rt->stack, loc);

    loc += VAL_SIZE_BYTES;

    for (i = 0; i < cap_count; ++i) {
        loc = rt_val_fun_next_cap_loc(rt, loc);
    }

    *appl_start = loc;
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

