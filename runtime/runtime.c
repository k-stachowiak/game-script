/* Copyright (C) 2014 Krzysztof Stachowiak */

#include <stdlib.h>
#include <inttypes.h>

#include "stack.h"
#include "eval.h"
#include "error.h"
#include "runtime.h"
#include "rt_val.h"

static void rt_free_stored(struct Runtime *rt)
{
	if (rt->node_store) {
		ast_node_free(rt->node_store);
	}
	rt->node_store = NULL;
}

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

    def = (struct AstNode*)rt_peek_ptr(rt, impl_loc);

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

static void rt_init(struct Runtime *rt, long stack)
{
	rt->stack = stack_make(stack);
	sym_map_init_global(&rt->global_sym_map, rt); /* TODO: This is evil! result is incomplete! */
	rt->node_store = NULL;
}

static void rt_deinit(struct Runtime *rt)
{
	rt_free_stored(rt);
	sym_map_deinit(&rt->global_sym_map);
	stack_free(rt->stack);
}

struct Runtime *rt_make(long stack)
{
    struct Runtime *result = malloc_or_die(sizeof(*result));
    rt_init(result, stack);
    return result;
}

void rt_reset(struct Runtime *rt)
{
	long stack = rt->stack->size;
	rt_deinit(rt);
	rt_init(rt, stack);
}

void rt_free(struct Runtime *rt)
{
	rt_deinit(rt);
    free_or_die(rt);
}

void rt_save(struct Runtime *rt)
{
	rt->saved_loc = rt->stack->top;
	rt->saved_store = rt->node_store;
}

void rt_restore(struct Runtime *rt)
{
	while (rt->node_store != rt->saved_store) {
		struct AstNode *temp = rt->node_store;
		ast_node_free_one(temp);
		rt->node_store = rt->node_store->next;
	}

	stack_collapse(rt->stack, rt->saved_loc, rt->stack->top);
}

void rt_consume_one(
        struct Runtime *rt,
        struct AstNode *ast,
        VAL_LOC_T *loc,
        struct AstNode **next)
{
	if (loc) {
		*loc = rt->stack->top;
	}
	if (next) {
		*next = ast->next;
	}

	eval(ast, rt, &rt->global_sym_map);

	if (err_state()) {
		return;

	} else if (ast->type == AST_BIND) {
		ast->next = rt->node_store;
		rt->node_store = ast;
	} else {
		rt->stack->top = *loc;
		ast_node_free_one(ast);
	}
}

bool rt_consume_list(struct Runtime *rt, struct AstNode *ast_list)
{
	struct AstNode *next;

	rt_save(rt);

	while (ast_list) {
		rt_consume_one(rt, ast_list, NULL, &next);
		if (err_state()) {
			rt_restore(rt);
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

void rt_for_each_stack_val(struct Runtime *rt, void(*f)(void*, VAL_LOC_T))
{
	stack_for_each(rt->stack, rt, f);
}

void rt_for_each_sym(struct Runtime *rt, void(*f)(void*, char*, VAL_LOC_T))
{
	sym_map_for_each(&rt->global_sym_map, rt, f);
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
        printf("%" PRId64 , rt_val_peek_int(rt, loc));
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

/** Peek a size_t at a given location */
VAL_SIZE_T rt_peek_size(struct Runtime *rt, VAL_LOC_T loc)
{
    VAL_SIZE_T result;
    memcpy(&result, rt->stack->buffer + loc, VAL_SIZE_BYTES);
    return result;
}

/** Peek a ptr_t at a given location */
void *rt_peek_ptr(struct Runtime *rt, VAL_LOC_T loc)
{
    void *result;
    memcpy((char*)&result, rt->stack->buffer + loc, VAL_PTR_BYTES);
    return result;
}

