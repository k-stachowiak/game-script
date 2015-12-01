/* Copyright (C) 2015 Krzysztof Stachowiak */

#include <stdbool.h>
#include <inttypes.h>

#include "error.h"
#include "eval.h"
#include "eval_detail.h"
#include "rt_val.h"

void para_error_invalid_argc(char *func, int count)
{
    err_push("EVAL",
        "Incorrect arguments count passed to _%s_: %d",
        func, count);
}

void para_error_arg_expected(char *func, int index, char *expected)
{
    err_push("EVAL",
            "Argument %d of parafunc _%s_ must be %s",
            index, func, expected);
}

void eval_parafunc(
        struct AstParafunc *parafunc,
        struct Runtime *rt,
        struct SymMap *sym_map)
{
    enum AstParafuncType type = parafunc->type;
    struct AstNode *args = parafunc->args;

    switch (type) {
    case AST_PARAFUNC_IF:
        eval_parafunc_if(rt, sym_map, args);
        break;

    case AST_PARAFUNC_WHILE:
        eval_parafunc_while(rt, sym_map, args);
        break;

    case AST_PARAFUNC_AND:
        eval_parafunc_logic(rt, sym_map, args, false, "and");
        break;

    case AST_PARAFUNC_OR:
        eval_parafunc_logic(rt, sym_map, args, true, "or");
        break;

    case AST_PARAFUNC_REF:
        eval_parafunc_ref(rt, sym_map, args);
        break;

    case AST_PARAFUNC_PEEK:
        eval_parafunc_peek(rt, sym_map, args);
        break;

    case AST_PARAFUNC_POKE:
        eval_parafunc_poke(rt, sym_map, args);
        break;

    case AST_PARAFUNC_BEGIN:
        eval_parafunc_begin(rt, sym_map, args);
        break;

    case AST_PARAFUNC_END:
        eval_parafunc_end(rt, sym_map, args);
        break;

    case AST_PARAFUNC_INC:
        eval_parafunc_inc(rt, sym_map, args);
        break;

    case AST_PARAFUNC_SUCC:
        eval_parafunc_succ(rt, sym_map, args);
        break;
    }
}
