/* Copyright (C) 2014,2015 Krzysztof Stachowiak */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "term.h"
#include "repl_cmd.h"
#include "repl_expr.h"
#include "runtime.h"
#include "error.h"
#include "memory.h"

static bool request_quit = false;

static void repl_handle_command(struct Runtime *rt, char *cmd)
{
    switch (repl_cmd_command(rt, cmd)) {
    case REPL_CMD_OK:
        break;

    case REPL_CMD_ERROR:
    case REPL_CMD_INTERNAL_ERROR:
        printf("Command error : %s.\n", err_msg());
        break;

    case REPL_CMD_QUIT:
        request_quit = true;
        break;
    }
}

static void repl_handle_expression(struct Runtime *rt, char *expr)
{
    switch (repl_expr_command(rt, expr)) {
    case REPL_EXPR_OK:
        break;

    case REPL_EXPR_ERROR:
    case REPL_EXPR_INTERNAL_ERROR:
        printf("Expr error : %s.\n", err_msg());
        break;
    }
}

static void repl_handle_line(struct Runtime *rt, char *line)
{
    if (strcmp(line, "") == 0) {
        return;
    } else if (*line == ':') {
        repl_handle_command(rt, line + 1);
    } else {
        repl_handle_expression(rt, line);
    }
}

int repl(int argc, char *argv[])
{
    int result;
    bool eof_flag = false;
    struct Runtime *rt = rt_make(64 * 1024);

    if (argc != 1) {
        printf("REPL: ignored additional command line arguments.\n");
    }

    repl_cmd_init(rt);

    for (;;) {

        char *line;

        err_reset();

        printf("moon > ");

        line = my_getline(&eof_flag);

        if (err_state()) {
            printf("IO error in line \"%s\" : \"%s\".\n", line, err_msg());
            result = 1;
            mem_free(line);
            break;

        } else if (eof_flag) {
            result = 0;
            mem_free(line);
            break;
        }

        repl_handle_line(rt, line);
        mem_free(line);

        if (request_quit) {
            result = 0;
            break;
        }
    }

    repl_cmd_deinit();

    rt_free(rt);

    if (result != 0) {
        printf("REPL error: %s\n", err_msg());
    } else {
        printf("REPL terminated correctly.\n");
    }

    return result;
}
