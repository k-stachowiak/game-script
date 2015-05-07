/* Copyright (C) 2014,2015 Krzysztof Stachowiak */

#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "memory.h"
#include "strbuild.h"
#include "log.h"
#include "runtime.h"
#include "rt_val.h"

static VAL_HEAD_SIZE_T zero = 0;

/* Writing / pushing.
 * ==================
 */

void rt_val_push_bool(struct Stack *stack, VAL_BOOL_T value)
{
    VAL_HEAD_TYPE_T type = (VAL_HEAD_TYPE_T)VAL_BOOL;
    VAL_HEAD_SIZE_T size = VAL_BOOL_BYTES;
    VAL_BOOL_T normalized_value = !!value;
    stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type);
    stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&size);
    stack_push(stack, size, (char*)&normalized_value);
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

void rt_val_push_string_as_array(struct Stack *stack, char *value)
{
    VAL_LOC_T data_begin, size_loc;
    rt_val_push_array_init(stack, &size_loc);
    data_begin = stack->top;
    while (*value) {
        rt_val_push_char(stack, *value);
        ++value;
    }
    rt_val_push_cpd_final(stack, size_loc, stack->top - data_begin);
}

void rt_val_push_string_slice_as_array(struct Stack *stack, char *value, int len)
{
    VAL_LOC_T data_begin,  size_loc;
    rt_val_push_array_init(stack, &size_loc);
    data_begin = stack->top;
    while (len) {
        rt_val_push_char(stack, *value);
        ++value;
        --len;
    }
    rt_val_push_cpd_final(stack, size_loc, stack->top - data_begin);
}

void rt_val_push_array_init(struct Stack *stack, VAL_LOC_T *size_loc)
{
    VAL_HEAD_TYPE_T type = (VAL_HEAD_TYPE_T)VAL_ARRAY;
    stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type);
    *size_loc = stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&zero);
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
        VAL_SIZE_T arity,
        void *ast_def,
        void *bif_impl)
{
    static VAL_HEAD_TYPE_T type = (VAL_HEAD_TYPE_T)VAL_FUNCTION;
    stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type);
    *size_loc = stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&zero);
    *data_begin = stack_push(stack, VAL_SIZE_BYTES, (char*)&arity);
    stack_push(stack, VAL_PTR_BYTES, (char*)&ast_def);
    stack_push(stack, VAL_PTR_BYTES, (char*)&bif_impl);
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

void rt_val_push_func_final(
        struct Stack *stack,
        VAL_LOC_T size_loc,
        VAL_SIZE_T data_begin)
{
    VAL_LOC_T data_size = stack->top - data_begin;
    memcpy(stack->buffer + size_loc, &data_size, VAL_HEAD_SIZE_BYTES);
}

/* Hacking (poking) API.
 * =====================
 */

void rt_val_poke_bool(struct Stack *stack, VAL_LOC_T loc, VAL_BOOL_T value)
{
    memcpy(stack->buffer + loc + VAL_HEAD_BYTES,
           (char*)&value,
           VAL_BOOL_BYTES);
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
        } else {
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
        } else {
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

    result.ast_def_loc = loc;
    loc += VAL_PTR_BYTES;

    result.bif_impl_loc = loc;
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
    result.ast_def = (struct AstNode*)stack_peek_ptr(rt->stack, result.ast_def_loc);
    result.bif_impl = (void*)stack_peek_ptr(rt->stack, result.bif_impl_loc);
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

static bool rt_val_pair_homo(struct Runtime *rt, VAL_LOC_T x, VAL_LOC_T y);

static bool rt_val_pair_homo_simple(struct Runtime *rt, VAL_LOC_T x, VAL_LOC_T y)
{
    struct ValueHeader
        header_x = rt_val_peek_header(rt->stack, x),
        header_y = rt_val_peek_header(rt->stack, y);

    return (
        (header_x.type == VAL_BOOL && header_y.type == VAL_BOOL) ||
        (header_x.type == VAL_CHAR && header_y.type == VAL_CHAR) ||
        (header_x.type == VAL_INT && header_y.type == VAL_INT) ||
        (header_x.type == VAL_REAL && header_y.type == VAL_REAL) ||
        (header_x.type == VAL_FUNCTION && header_y.type == VAL_FUNCTION)
    );
}

static bool rt_val_pair_homo_complex(struct Runtime *rt, VAL_LOC_T x, VAL_LOC_T y)
{
    VAL_LOC_T current_x, current_y;
    int i, len_x, len_y;

    struct ValueHeader
        header_x = rt_val_peek_header(rt->stack, x),
        header_y = rt_val_peek_header(rt->stack, y);

    if (header_x.type == VAL_ARRAY && header_y.type == VAL_ARRAY) {

        /* In case of arrays we only compare the first elements as the homogenity
         * is assured by the language rules in static and dynamic checks.
         * Note that empty array will type-match any other array.
         */

        len_x = rt_val_cpd_len(rt, x);
        len_y = rt_val_cpd_len(rt, y);

        if (len_x == 0 || len_y == 0) {
            return true;

        } else {
            current_x = rt_val_cpd_first_loc(x);
            current_y = rt_val_cpd_first_loc(y);
            return rt_val_pair_homo(rt, current_x, current_y);

        }

    } else if (header_x.type == VAL_TUPLE && header_y.type == VAL_TUPLE) {

        /* In case of tuples, we must compare the element counts as well as
         * all the contents element-wise.
         */

        len_x = rt_val_cpd_len(rt, x);
        len_y = rt_val_cpd_len(rt, y);

        if (len_x != len_y) {
            return false;

        } else {
            current_x = rt_val_cpd_first_loc(x);
            current_y = rt_val_cpd_first_loc(y);

            for (i = 0; i < len_x; ++i) {
                if (!rt_val_pair_homo(rt, current_x, current_y)) {
                    return false;
                }
                current_x = rt_val_next_loc(rt, current_x);
                current_y = rt_val_next_loc(rt, current_y);
            }

            return true;
        }

    } else {
        return false;

    }
}

static bool rt_val_pair_homo(struct Runtime *rt, VAL_LOC_T x, VAL_LOC_T y)
{
    return rt_val_pair_homo_simple(rt, x, y) ||
           rt_val_pair_homo_complex(rt, x, y);
}

bool rt_val_compound_homo(struct Runtime *rt, VAL_LOC_T val_loc)
{
    int i, len = rt_val_cpd_len(rt, val_loc);
    VAL_LOC_T first, current;

    if (len < 2) {
        return true;
    }

    first = rt_val_cpd_first_loc(val_loc);
    current = rt_val_next_loc(rt, first);

    for (i = 1; i < len; ++i) {
        if (!rt_val_pair_homo(rt, first, current)) {
            return false;
        }
        current = rt_val_next_loc(rt, current);
    }

    return true;
}

bool rt_val_eq_rec(struct Runtime *rt, VAL_LOC_T x, VAL_LOC_T y)
{
    enum ValueType xtype, ytype;
    int xlen, ylen;

    if (rt_val_peek_size(rt, x) != rt_val_peek_size(rt, y)) {
        return false;
    }

    xtype = rt_val_peek_type(rt, x);
    ytype = rt_val_peek_type(rt, y);

    if (xtype != ytype) {
        return false;
    }

    switch (xtype) {
    case VAL_BOOL:
        return rt_val_peek_bool(rt, x) == rt_val_peek_bool(rt, y);

    case VAL_CHAR:
        return rt_val_peek_char(rt, x) == rt_val_peek_char(rt, y);

    case VAL_INT:
        return rt_val_peek_int(rt, x) == rt_val_peek_int(rt, y);

    case VAL_REAL:
        return rt_val_peek_real(rt, x) == rt_val_peek_real(rt, y);

    case VAL_ARRAY:
    case VAL_TUPLE:
        xlen = rt_val_cpd_len(rt, x);
        ylen = rt_val_cpd_len(rt, y);
        if (xlen != ylen) {
            return false;
        }
        x = rt_val_cpd_first_loc(x);
        y = rt_val_cpd_first_loc(y);
        while (xlen) {
            if (!rt_val_eq_rec(rt, x, y)) {
                return false;
            }
            x = rt_val_next_loc(rt, x);
            y = rt_val_next_loc(rt, y);
            --xlen;
        }
        return true;

    case VAL_FUNCTION:
        return false;
    }

    LOG_ERROR("Cannot get here");
    exit(1);
}

bool rt_val_eq_bin(struct Runtime *rt, VAL_LOC_T x, VAL_LOC_T y)
{
    VAL_SIZE_T x_size = rt_val_peek_size(rt, x);
    VAL_SIZE_T y_size = rt_val_peek_size(rt, y);

    if (x_size != y_size) {
        return false;
    }

    return memcmp(
            rt->stack->buffer + x,
            rt->stack->buffer + y,
            x_size + VAL_HEAD_BYTES) == 0;
}

bool rt_val_string_eq(struct Runtime *rt, VAL_LOC_T loc, char *str)
{
    char *stack_str = rt_val_peek_cpd_as_string(rt, loc);
    bool result = strcmp(stack_str, str) == 0;
    mem_free(stack_str);
    return result;
}
