/* Copyright (C) 2014 Krzysztof Stachowiak */

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>

#include "error.h"
#include "lex.h"
#include "parse.h"
#include "common.h"
#include "dom.h"
#include "repl_cmd.h"
#include "repl_state.h"

static struct DomNode *repl_cmd_load_dom_list(char *filename)
{
	struct DomNode *dom_list;
	char *source;

	source = my_getfile(filename);

	if (!source) {
		printf("Failed loading file \"%s\".\n", filename);
		return NULL;
	}

	if (!(dom_list = lex(source))) {
		printf("Failed lexing file \"%s\".\n", filename);
		return NULL;
	}

	return dom_list;
}

static struct AstNode *repl_cmd_load_ast_list(
		char *filename,
		struct DomNode *dom_list)
{
	struct AstNode *ast_list = NULL;

	for (; dom_list; dom_list = dom_list->next) {
		struct AstNode *ast;
		if (!(ast = parse(dom_list))) {
			printf("Failed parsing file \"%s\".\n", filename);
			ast_node_free(ast_list);
			return NULL;
		}
		else {
			ast->next = ast_list;
			ast_list = ast;
		}
	}

	return ast_list;
}

static bool repl_cmd_load_consume_ast(
		char *filename,
		struct AstNode *ast_list)
{
	repl_state_save();
	for (; ast_list; ast_list = ast_list->next) {
		repl_state_consume(ast_list);
		if (err_state()) {
			printf("Failed evaluating file \"%s\".\n", filename);
			repl_state_restore();
			/* NOTE: ast_list is used for the iteration so upon error,
			* no nodes that have already been consumed will be freed.
			* They have actually been freed upon success of the consume function.
			*/
			ast_node_free(ast_list);
			return false;
		}
	}
	return true;
}

static bool repl_cmd_load(char *pieces[], int num_pieces)
{
	char *filename;

	struct DomNode *dom_list;
	struct AstNode *ast_list;

	if (num_pieces != 1) {
		printf("Load command expects 1 argument.\n");
		return false;
	}

	filename = pieces[0];

	return
		(dom_list = repl_cmd_load_dom_list(filename)) &&
		(ast_list = repl_cmd_load_ast_list(filename, dom_list)) &&
		(repl_cmd_load_consume_ast(filename, ast_list));
}

static void repl_cmd_print_stack(void)
{

}

static void repl_cmd_print_sym_map(void)
{

}

static enum ReplCmdResult bool2result(bool x)
{
	if (x) {
		return REPL_CMD_OK;
	} else {
		return REPL_CMD_ERROR;
	}
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
		printf("No command provided.\n");
		return REPL_CMD_ERROR;
	}

	if (strcmp(pieces[0], "q") == 0) {
		return REPL_CMD_QUIT;

	} else if (strcmp(pieces[0], "ld") == 0) {
		return bool2result(repl_cmd_load(pieces + 1, num_pieces - 1));

	} else if (strcmp(pieces[0], "pst") == 0) {
		repl_cmd_print_stack();

	} else if (strcmp(pieces[0], "psm") == 0) {
		repl_cmd_print_sym_map();

	} else {
		printf("Bad command or file name.\n");
		return REPL_CMD_ERROR;
	}

	return REPL_CMD_OK;
}
