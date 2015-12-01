/* Copyright (C) 2014,2015 Krzysztof Stachowiak */

#include <string.h>

#include "ast.h"
#include "runtime.h"
#include "symmap.h"

void eval_literal(struct AstLiteral *literal, struct Runtime *rt, struct SymMap *sym_map)
{
    char *string;
    int string_len;

    (void)sym_map;

    switch (literal->type) {
    case AST_LIT_UNIT:
        rt_val_push_unit(&rt->stack);
        break;

    case AST_LIT_BOOL:
        rt_val_push_bool(&rt->stack, literal->data.boolean);
        break;

    case AST_LIT_CHAR:
        rt_val_push_char(&rt->stack, literal->data.character);
        break;

    case AST_LIT_INT:
        rt_val_push_int(&rt->stack, literal->data.integer);
        break;

    case AST_LIT_REAL:
        rt_val_push_real(&rt->stack, literal->data.real);
        break;

    case AST_LIT_STRING:
        string = literal->data.string;
        string_len = strlen(string);
        rt_val_push_string(&rt->stack, string + 1, string + string_len - 1);
        break;
    }
}

