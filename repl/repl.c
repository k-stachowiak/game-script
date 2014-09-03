/* Copyright (C) 2014 Krzysztof Stachowiak */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "repl_cmd.h"
#include "repl_expr.h"
#include "repl_state.h"
#include "error.h"

static bool request_quit = false;

static void repl_handle_command(char *cmd)
{
	switch (repl_cmd_command(cmd)) {
	case REPL_CMD_ERROR:
	case REPL_CMD_OK:
		break;

	case REPL_CMD_QUIT:
		request_quit = true;
		break;
	}
}

static void repl_handle_expression(char *expr)
{
	switch (repl_expr_command(expr)) {
	case REPL_EXPR_OK:
	case REPL_EXPR_ERROR:
		break;
	}
}

static void repl_handle_line(char *line)
{
    if (strcmp(line, "") == 0) {
        return;

	} else if (*line == ':') {
		repl_handle_command(line + 1);

	} else {
		repl_handle_expression(line);
	}
}

int repl(void)
{
	int result;
    bool eof_flag = false;

    err_reset();
	repl_state_init();

    for (;;) {

        char *line;

        printf("moon> ");
        line = my_getline(&eof_flag);

        if (err_state()) {
            result = 1;
			free(line);
			break;

        } else if (eof_flag) {
			result = 0;
			free(line);
			break;
        }

        repl_handle_line(line);
		free(line);

        if (request_quit) {
			result = 0;
			break;
		}
    }

	repl_state_deinit();
	printf("\n");
	return result;
}
