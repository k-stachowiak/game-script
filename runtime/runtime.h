/* Copyright (C) 2014,2015 Krzysztof Stachowiak */

#ifndef RUNTIME_H
#define RUNTIME_H

#include <stdbool.h>

#include "ast.h"
#include "stack.h"
#include "symmap.h"

typedef void (*EvalCallback)(void*, struct AstNode*);

struct Runtime {
    struct Stack *stack;
    struct SymMap global_sym_map;
    struct AstNode *node_store;
    VAL_LOC_T saved_loc;
    struct AstNode *saved_store;
	void *eval_callback_data;
	EvalCallback eval_callback_begin;
	EvalCallback eval_callback_end;
};

struct Runtime *rt_make(long stack);
void rt_reset(struct Runtime *rt);
void rt_free(struct Runtime *rt);

void rt_save(struct Runtime *rt);
void rt_restore(struct Runtime *rt);

void rt_set_eval_callback(
		struct Runtime *rt,
		void *data,
		EvalCallback begin,
		EvalCallback end);

void rt_consume_one(
        struct Runtime *rt,
        struct AstNode *ast,
        VAL_LOC_T *loc,
        struct AstNode **next);

bool rt_consume_list(struct Runtime *rt, struct AstNode *ast);

void rt_for_each_stack_val(struct Runtime *rt, void(*f)(void*, VAL_LOC_T));
void rt_for_each_sym(struct Runtime *rt, void(*f)(void*, char*, VAL_LOC_T));

#endif
