/* Copyright (C) 2014 Krzysztof Stachowiak */

#include <stdlib.h>
#include <inttypes.h>

#include "stack.h"
#include "eval.h"
#include "error.h"
#include "runtime.h"

static struct Stack *stack;
static struct SymMap sym_map;
static struct AstNode *node_store;

static VAL_LOC_T saved_loc;
static struct AstNode *saved_store;

static void repl_free_bound(void)
{
	if (node_store) {
		ast_node_free(node_store);
	}
	node_store = NULL;
}

void rt_init(void)
{
	stack = stack_make(64 * 1024);
	sym_map_init_global(&sym_map, stack);
	node_store = NULL;
}

void rt_deinit(void)
{
	repl_free_bound();
	sym_map_deinit(&sym_map);
	stack_free(stack);
}

void rt_save(void)
{
	saved_loc = stack->top;
	saved_store = node_store;
}

void rt_restore(void)
{
	while (node_store != saved_store) {
		struct AstNode *temp = node_store;
		ast_node_free_one(temp);
		node_store = node_store->next;
	}

	stack_collapse(stack, saved_loc, stack->top);
}

VAL_LOC_T rt_current_top(void)
{
	return stack->top;
}

void rt_consume_one(struct AstNode *ast, VAL_LOC_T *loc, struct AstNode **next)
{
	if (loc) {
		*loc = stack->top;
	}
	if (next) {
		*next = ast->next;
	}

	eval(ast, stack, &sym_map);

	if (err_state()) {
		return;

	} else if (ast->type == AST_BIND) {
		ast->next = node_store;
		node_store = ast;

	} else {
		stack->top = *loc;
		ast_node_free_one(ast);

	}
}

bool rt_consume_list(struct AstNode *ast_list)
{
	struct AstNode *next;

	rt_save();

	while (ast_list) {
		rt_consume_one(ast_list, NULL, &next);
		if (err_state()) {
			rt_restore();
			/* NOTE: ast_list is used for the iteration so upon error,
			* no nodes that have already been consumed will be freed.
			* They have actually been freed upon success of the consume function.
			*/
			ast_node_free(ast_list);
			return false;
		}
		ast_list = next;
	}
	return true;
}

void rt_for_each_stack_val(void(*f)(VAL_LOC_T, struct Value*))
{
	stack_for_each(stack, f);
}

void rt_for_each_sym(void(*f)(char*, VAL_LOC_T))
{
	sym_map_for_each(&sym_map, f);
}

struct Value rt_peek(VAL_LOC_T location)
{
	return stack_peek_value(stack, location);
}

static void rt_val_print_compound(
        VAL_LOC_T loc, char open, char close)
{
    VAL_LOC_T cpd_loc, end;

    printf("%c ", open);

    cpd_loc = rt_peek_val_cpd_first(loc);
    end = cpd_loc + rt_val_size(loc);
    while (cpd_loc != end) {
        rt_val_print(cpd_loc, false);
        printf(" ");
        cpd_loc = rt_next_loc(cpd_loc);
    }

    printf("%c", close);
}

static void rt_val_print_function(VAL_LOC_T loc)
{
    VAL_LOC_T impl_loc, cap_start, appl_start;
    VAL_SIZE_T captures, applied, arity;
    struct AstNode *def;

    rt_peek_val_fun_locs(loc, &impl_loc, &cap_start, &appl_start);

    def = (struct AstNode*)rt_peek_ptr(impl_loc);

    if (def->type == AST_BIF) {
        printf("built-in function");
        return;
    }

    if (def->type != AST_FUNC_DEF) {
		LOG_ERROR("Attempting to print non-func value with func printing.\n");
        exit(1);
    }

    captures = rt_val_size(cap_start);
    applied = rt_val_size(appl_start);
    arity = def->data.func_def.func.arg_count - applied;
    printf("function (ar=%d, cap=%d, appl=%d)", arity, captures, applied);
}

void rt_val_print(VAL_LOC_T loc, bool annotate)
{
    switch (rt_val_type(loc)) {
    case VAL_BOOL:
        if (annotate) {
            printf("bool :: ");
        }
        printf("%s", rt_peek_val_bool(loc) ? "true" : "false");
        break;

    case VAL_CHAR:
        if (annotate) {
            printf("char :: ");
        }
        printf("'%c'", rt_peek_val_char(loc));
        break;

    case VAL_INT:
        if (annotate) {
            printf("integer :: ");
        }
        printf("%" PRId64 , rt_peek_val_int(loc));
        break;

    case VAL_REAL:
        if (annotate) {
            printf("real :: ");
        }
        printf("%f", rt_peek_val_real(loc));
        break;

    case VAL_STRING:
        if (annotate) {
            printf("string :: ");
        }
        printf("%s", rt_peek_val_string(loc));
        break;

    case VAL_ARRAY:
        if (annotate) {
            printf("array :: ");
        }
        rt_val_print_compound(loc, '[', ']');
        break;

    case VAL_TUPLE:
        if (annotate) {
            printf("tuple :: ");
        }
        rt_val_print_compound(loc, '{', '}');
        break;

    case VAL_FUNCTION:
        rt_val_print_function(loc);
        break;
    }
}

VAL_LOC_T rt_next_loc(VAL_LOC_T loc)
{
    struct ValueHeader header = stack_peek_header(stack, loc);
    return loc + VAL_HEAD_BYTES + header.size;
}

enum ValueType rt_val_type(VAL_LOC_T loc)
{
    struct ValueHeader header = stack_peek_header(stack, loc);
    return (enum ValueType)header.type;
}

VAL_SIZE_T rt_val_size(VAL_LOC_T loc)
{
    VAL_SIZE_T result;
    memcpy(&result, stack->buffer + loc + VAL_HEAD_TYPE_BYTES, VAL_SIZE_BYTES);
    return result;
}

VAL_SIZE_T rt_peek_size(VAL_LOC_T loc)
{
    VAL_SIZE_T result;
    memcpy(&result, stack->buffer + loc, VAL_SIZE_BYTES);
    return result;
}

void *rt_peek_ptr(VAL_LOC_T loc)
{
    void *result;
    memcpy((char*)&result, stack->buffer + loc, VAL_PTR_BYTES);
    return result;
}

VAL_BOOL_T rt_peek_val_bool(VAL_LOC_T loc)
{
    VAL_BOOL_T result;
    memcpy(&result, stack->buffer + loc + VAL_HEAD_BYTES, VAL_BOOL_BYTES);
    return result;
}

VAL_CHAR_T rt_peek_val_char(VAL_LOC_T loc)
{
    VAL_CHAR_T result;
    memcpy(&result, stack->buffer + loc + VAL_HEAD_BYTES, VAL_CHAR_BYTES);
    return result;
}

VAL_INT_T rt_peek_val_int(VAL_LOC_T loc)
{
    VAL_INT_T result;
    memcpy(&result, stack->buffer + loc + VAL_HEAD_BYTES, VAL_INT_BYTES);
    return result;
}

VAL_REAL_T rt_peek_val_real(VAL_LOC_T loc)
{
    VAL_REAL_T result;
    memcpy(&result, stack->buffer + loc + VAL_HEAD_BYTES, VAL_REAL_BYTES);
    return result;
}

char* rt_peek_val_string(VAL_LOC_T loc)
{
    return stack->buffer + loc + VAL_HEAD_BYTES;
}

VAL_LOC_T rt_peek_val_cpd_first(VAL_LOC_T loc)
{
    return loc + VAL_HEAD_BYTES;
}

void rt_peek_val_fun_locs(
        VAL_LOC_T loc,
        VAL_LOC_T *impl_loc,
        VAL_LOC_T *cap_start,
        VAL_LOC_T *appl_start)
{
    int cap_count, i;
    loc += VAL_HEAD_BYTES;

    *impl_loc = loc;
    loc = rt_next_loc(loc);

    *cap_start = loc;
    cap_count = rt_val_size(loc);
    loc += VAL_SIZE_BYTES;

    for (i = 0; i < cap_count; ++i) {
        loc = rt_next_loc(loc);
    }

    *appl_start = loc;
}

char *rt_fun_cap_symbol(VAL_LOC_T cap_loc)
{
    return stack->buffer + cap_loc; 
}

VAL_LOC_T rt_fun_cap_val_loc(VAL_LOC_T cap_loc)
{
    VAL_SIZE_T len = strlen(rt_fun_cap_symbol(cap_loc));
    return cap_loc + len + 1;
}

VAL_LOC_T rt_fun_next_cap_loc(VAL_LOC_T loc)
{
    return rt_next_loc(rt_fun_cap_val_loc(loc));
}

VAL_LOC_T rt_fun_next_appl_loc(VAL_LOC_T loc)
{
    return rt_next_loc(loc);
}

