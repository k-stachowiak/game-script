/* Copyright (C) 2014 Krzysztof Stachowiak */

#ifndef REPL_CMD_H
#define REPL_CMD_H

#include "runtime.h"

enum ReplCmdResult {
	REPL_CMD_OK,
	REPL_CMD_ERROR,
	REPL_CMD_INTERNAL_ERROR,
	REPL_CMD_QUIT
};

void repl_cmd_init(struct Runtime* rt);
void repl_cmd_deinit(void);

enum ReplCmdResult repl_cmd_command(struct Runtime *rt, char *command_line);

#endif
