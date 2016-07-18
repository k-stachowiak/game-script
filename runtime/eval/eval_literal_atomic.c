/* Copyright (C) 2014,2015 Krzysztof Stachowiak */

#include <string.h>

#include "ast.h"
#include "runtime.h"
#include "symmap.h"

void eval_literal_atomic(
        struct AstLiteralAtomic *literal_atomic,
        struct Runtime *rt,
        struct SymMap *sym_map)
{
    char *string;
    int string_len;

    (void)sym_map;

    switch (literal_atomic->type) {
    case AST_LIT_ATOM_UNIT:
        rt_val_push_unit(&rt->stack);
        break;

    case AST_LIT_ATOM_BOOL:
        rt_val_push_bool(&rt->stack, literal_atomic->data.boolean);
        break;

    case AST_LIT_ATOM_CHAR:
        rt_val_push_char(&rt->stack, literal_atomic->data.character);
        break;

    case AST_LIT_ATOM_INT:
        rt_val_push_int(&rt->stack, literal_atomic->data.integer);
        break;

    case AST_LIT_ATOM_REAL:
        rt_val_push_real(&rt->stack, literal_atomic->data.real);
        break;

    case AST_LIT_ATOM_STRING:
        string = literal_atomic->data.string;
        string_len = strlen(string);
        rt_val_push_string(&rt->stack, string + 1, string + string_len - 1);
        break;
    }
}

