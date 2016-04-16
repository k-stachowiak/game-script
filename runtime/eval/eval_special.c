/* Copyright (C) 2015 Krzysztof Stachowiak */

#include <stdbool.h>
#include <inttypes.h>

#include "error.h"
#include "eval.h"
#include "eval_detail.h"
#include "rt_val.h"

void spec_error_invalid_argc(char *func, int count)
{
    err_push("EVAL",
        "Incorrect arguments count passed to _%s_: %d",
        func, count);
}

void spec_error_arg_expected(char *func, int index, char *expected)
{
    err_push("EVAL",
            "Argument %d of special form _%s_ must be %s",
            index, func, expected);
}

void eval_special(
        struct AstSpecial *special,
        struct Runtime *rt,
        struct SymMap *sym_map)
{
    enum AstSpecialType type = special->type;
    struct AstNode *args = special->args;

    switch (type) {
    case AST_SPECIAL_IF:
        eval_special_if(rt, sym_map, args);
        break;

    case AST_SPECIAL_WHILE:
        eval_special_while(rt, sym_map, args);
        break;

    case AST_SPECIAL_AND:
        eval_special_logic(rt, sym_map, args, false, "and");
        break;

    case AST_SPECIAL_OR:
        eval_special_logic(rt, sym_map, args, true, "or");
        break;

    case AST_SPECIAL_REF:
        eval_special_ref(rt, sym_map, args);
        break;

    case AST_SPECIAL_PEEK:
        eval_special_peek(rt, sym_map, args);
        break;

    case AST_SPECIAL_POKE:
        eval_special_poke(rt, sym_map, args);
        break;

    case AST_SPECIAL_BEGIN:
        eval_special_begin(rt, sym_map, args);
        break;

    case AST_SPECIAL_END:
        eval_special_end(rt, sym_map, args);
        break;

    case AST_SPECIAL_INC:
        eval_special_inc(rt, sym_map, args);
        break;

    case AST_SPECIAL_SUCC:
        eval_special_succ(rt, sym_map, args);
        break;
    }
}
