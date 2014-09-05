/* Copyright (C) 2014 Krzysztof Stachowiak */

#include "value.h"

void repl_state_init(void);
void repl_state_deinit(void);

void repl_state_save(void);
void repl_state_restore(void);

VAL_LOC_T repl_state_current_top(void);
VAL_LOC_T repl_state_consume(struct AstNode *ast);
void repl_state_for_each_stack_val(void(*f)(VAL_LOC_T, struct Value*));
void repl_state_for_each_sym(void(*f)(char*, VAL_LOC_T));
struct Value repl_state_peek(VAL_LOC_T loc);