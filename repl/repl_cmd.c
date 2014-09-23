/* Copyright (C) 2014 Krzysztof Stachowiak */

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>

#include "error.h"
#include "common.h"
#include "repl_cmd.h"
#include "runtime.h"
#include "ast_parse.h"

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

static enum ReplCmdResult repl_cmd_load(char *pieces[], int num_pieces)
{
	char *filename;
	struct AstNode *ast_list;

	if (num_pieces != 1) {
        repl_cmd_error_args("load", 1, num_pieces);
		return REPL_CMD_ERROR;
	}

	filename = pieces[0];

	if (!(ast_list = ast_parse_file(filename))) {
		return REPL_CMD_INTERNAL_ERROR;
	}

	if (!rt_consume_list(ast_list)) {
		return REPL_CMD_INTERNAL_ERROR;
	}

	return REPL_CMD_OK;
}

void repl_cmd_print_stack_value(VAL_LOC_T loc, struct Value *val)
{
	val_print(val, true);
	printf(" @ %ld\n", (long)loc);
}

static void repl_cmd_print_stack(void)
{
	rt_for_each_stack_val(repl_cmd_print_stack_value);
}

static void repl_cmd_print_sym_map_kvp(char *symbol, VAL_LOC_T location)
{
	struct Value val = rt_peek(location);
	printf("%s -> ", symbol);
	val_print(&val, true);
	printf(" @ %ld\n", (long)location);
}

static void repl_cmd_print_sym_map(void)
{
	rt_for_each_sym(repl_cmd_print_sym_map_kvp);
}

enum ReplCmdResult repl_cmd_command(char *command_line)
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
		return repl_cmd_load(pieces + 1, num_pieces - 1);

	} else if (strcmp(pieces[0], "pst") == 0) {
		repl_cmd_print_stack();

	} else if (strcmp(pieces[0], "psm") == 0) {
		repl_cmd_print_sym_map();

	} else {
        repl_cmd_error_bad();
		return REPL_CMD_ERROR;
	}

	return REPL_CMD_OK;
}

