/* Copyright (C) 2014,2015 Krzysztof Stachowiak */

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>

#include "log.h"
#include "term.h"
#include "error.h"
#include "repl_cmd.h"
#include "runtime.h"
#include "parse.h"
#include "rt_val.h"
#include "dbg.h"

static struct Debugger dbg;
static bool dbg_enabled;

static void repl_cmd_error_nofile(void)
{
    struct ErrMessage msg;
    err_msg_init(&msg, "REPL");
    err_msg_append(&msg, "Failed loading a file");
    err_msg_set(&msg);
}

static void repl_cmd_error_bad(void)
{
    struct ErrMessage msg;
    err_msg_init(&msg, "REPL");
    err_msg_append(&msg, "Bad command or file name");
    err_msg_set(&msg);
}

static void repl_cmd_error_no(void)
{
    struct ErrMessage msg;
    err_msg_init(&msg, "REPL");
    err_msg_append(&msg, "No command provided");
    err_msg_set(&msg);
}

static void repl_cmd_error_args(char *cmd, int expected, int actual)
{
    struct ErrMessage msg;
    err_msg_init(&msg, "REPL");
    err_msg_append(&msg,
        "Invalid arguments to command \"%s\" : %d (expected %d)",
        cmd, actual, expected);
    err_msg_set(&msg);
}

static enum ReplCmdResult repl_cmd_load(
        struct Runtime *rt,
        char *pieces[],
        int num_pieces)
{
    char *filename;
    char *source;
    struct AstNode *ast_list;

    if (num_pieces != 1) {
        repl_cmd_error_args("load", 1, num_pieces);
        return REPL_CMD_ERROR;
    }

    filename = pieces[0];

    if (!(source = my_getfile(filename))) {
        repl_cmd_error_nofile();
        return REPL_CMD_INTERNAL_ERROR;
    }

    if (!(ast_list = parse_source(source))) {
        mem_free(source);
        return REPL_CMD_INTERNAL_ERROR;
    }

    if (!rt_consume_list(rt, ast_list, NULL)) {
        mem_free(source);
        return REPL_CMD_INTERNAL_ERROR;
    }

    mem_free(source);
    return REPL_CMD_OK;
}

static void repl_cmd_print_stack_value(void *state, VAL_LOC_T loc)
{
    struct Runtime *rt = (struct Runtime *)state;
    rt_val_print(rt, loc, true);
    printf(" @ %ld\n", (long)loc);
}

static void repl_cmd_print_stack(struct Runtime *rt)
{
    rt_for_each_stack_val(rt, repl_cmd_print_stack_value);
}

static void repl_cmd_print_sym_map_kvp(void *state, char *symbol, VAL_LOC_T location)
{
    struct Runtime *rt = (struct Runtime *)state;
    printf("%s -> ", symbol);
    rt_val_print(rt, location, true);
    printf(" @ %ld\n", (long)location);
}

static void repl_cmd_print_sym_map(struct Runtime *rt)
{
    rt_for_each_sym(rt, repl_cmd_print_sym_map_kvp);
}

static void repl_cmd_dbg_toggle(struct Runtime *rt)
{
    if (dbg_enabled) {
        rt_reset_eval_callback(rt);
        dbg_enabled = false;
        printf("debugger disabled\n");
    } else {
        rt_set_eval_callback(rt, &dbg, dbg_callback_begin, dbg_callback_end);
        dbg_enabled = true;
        printf("debugger enabled\n");
    }
}

void repl_cmd_init(struct Runtime *rt)
{
    static char *std_filename = "std.mn";

    dbg_init(&dbg);
    dbg_enabled = false;

    /* Preload standard library. */
    repl_cmd_load(rt, &std_filename, 1);

    /* Toggle the debugger on (_after_ initializing the standard library). */
    repl_cmd_dbg_toggle(rt);
}

void repl_cmd_deinit(void)
{
    dbg_deinit(&dbg);
}

enum ReplCmdResult repl_cmd_command(struct Runtime * rt, char *command_line)
{
    char *pieces[2];
    int num_pieces = 0;
    char *current = command_line;
    char *last = command_line + strlen(command_line);

    while (current != last) {
        pieces[num_pieces++] = current;
        while (*current && !isspace(*(current++)));
        *(current - 1) = '\0';
    }

    if (num_pieces == 0) {
        repl_cmd_error_no();
        return REPL_CMD_ERROR;
    }

    if (strcmp(pieces[0], "q") == 0) {
        return REPL_CMD_QUIT;

    } else if (strcmp(pieces[0], "ld") == 0) {
        return repl_cmd_load(rt, pieces + 1, num_pieces - 1);

    } else if (strcmp(pieces[0], "pst") == 0) {
        repl_cmd_print_stack(rt);

    } else if (strcmp(pieces[0], "psm") == 0) {
        repl_cmd_print_sym_map(rt);

    } else if (strcmp(pieces[0], "d") == 0) {
        repl_cmd_dbg_toggle(rt);

    } else {
        repl_cmd_error_bad();
        return REPL_CMD_ERROR;
    }

    return REPL_CMD_OK;
}

