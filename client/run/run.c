/* Copyright (C) 2014 Krzysztof Stachowiak */

#include <stdio.h>

#include "error.h"
#include "runtime.h"
#include "rt_val.h"
#include "parse.h"

int run(int argc, char *argv[])
{
    char *filename;
    struct AstNode *ast_list, *call_expr;
    struct Runtime *rt;
    VAL_LOC_T result_loc;

    if (argc == 1) {
        printf("RUN: Missing script filename argument.\n");
        return 1;
    }

    if (argc > 2) {
        printf("RUN: ignored additional command line arguments.\n");
    }

    filename = argv[1];
    if (!(ast_list = parse_file(filename))) {
        return 1;
    }

    rt = rt_make(64 * 1024);
    if (!rt_consume_list(rt, ast_list)) {
        printf("Interpreter error: %s\n", err_msg());
        rt_free(rt);
        return 1;
    }

    call_expr = parse_source("(main)");
    rt_consume_one(rt, call_expr, &result_loc, NULL);
    if (err_state()) {
        rt_free(rt);
        return 1;
    }

    rt_val_print(rt, result_loc, false);
    printf("\n");

    rt_free(rt);
    return 0;
}
