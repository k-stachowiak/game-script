/* Copyright (C) 2014 Krzysztof Stachowiak */

#ifndef RT_H
#define RT_H

#include <stdbool.h>

#include "value.h"

void rt_init(void);
void rt_deinit(void);

void rt_save(void);
void rt_restore(void);

VAL_LOC_T rt_current_top(void);

void rt_consume_one(struct AstNode *ast, VAL_LOC_T *loc, struct AstNode **next);
bool rt_consume_list(struct AstNode *ast);

void rt_for_each_stack_val(void(*f)(VAL_LOC_T, struct Value*));
void rt_for_each_sym(void(*f)(char*, VAL_LOC_T));
struct Value rt_peek(VAL_LOC_T loc);

#endif
