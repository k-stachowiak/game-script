/* Copyright (C) 2014-2016 Krzysztof Stachowiak */

#include <string.h>

#include "ast.h"
#include "runtime.h"
#include "symmap.h"

void eval_literal_atomic(
        struct AstNode *node,
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct AstLocMap *alm)
{
    char *string;
    int string_len;
    struct AstLiteralAtomic *literal_atomic = &node->data.literal_atomic;

    (void)sym_map;
    (void)alm;

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

    case AST_LIT_ATOM_DATATYPE:
        switch (literal_atomic->data.datatype) {
        case AST_LIT_ATOM_DATATYPE_VOID:
            rt_val_push_datatype_atom(&rt->stack, VAL_DATA_VOID);
            break;

        case AST_LIT_ATOM_DATATYPE_UNIT:
            rt_val_push_datatype_atom(&rt->stack, VAL_DATA_UNIT);
            break;

        case AST_LIT_ATOM_DATATYPE_BOOLEAN:
            rt_val_push_datatype_atom(&rt->stack, VAL_DATA_BOOLEAN);
            break;

        case AST_LIT_ATOM_DATATYPE_INTEGER:
            rt_val_push_datatype_atom(&rt->stack, VAL_DATA_INTEGER);
            break;

        case AST_LIT_ATOM_DATATYPE_REAL:
            rt_val_push_datatype_atom(&rt->stack, VAL_DATA_REAL);
            break;

        case AST_LIT_ATOM_DATATYPE_CHARACTER:
            rt_val_push_datatype_atom(&rt->stack, VAL_DATA_CHARACTER);
            break;
        }
        break;
    }
}

