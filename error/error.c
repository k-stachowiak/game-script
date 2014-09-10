/* Copyright (C) 2014 Krzysztof Stachowiak */

#include <stdlib.h>
#include <stdio.h>

#include "common.h"
#include "error.h"

static bool err_state_arr[ERR_MODULES_COUNT];
static char *err_msg_arr[ERR_MODULES_COUNT];

static err_internal_error(void)
{
	LOG_ERROR("Error in the error system. Too embarrassed to continue.\n");
	exit(2);
}

static char *err_module_to_string(enum ErrModule module)
{
	switch (module) {
	case ERR_TOK:
		return "TOKENIZE";

	case ERR_DOM:
		return "DOM-BUILD";

	case ERR_PARSE:
		return "PARSE";

	case ERR_REPL:
		return "REPL";

	case ERR_EVAL:
		return "EVAL";

	default:
		err_internal_error();
		return NULL;
	}
}

static char *err_build_message(
		enum ErrModule module,
		char *message,
		struct Location *location)
{
	char *result = malloc_or_die(4 * 1024);
	struct Location temp_location = { -66, -6 };

	/* TODO: Get rid of this ASAP. */
	if (!location) {
		location = &temp_location;
	}

	sprintf(result, "[%s] at %d-%d : %s",
		err_module_to_string(module),
		location->line, location->column,
		message);

	return result;
}

void err_free(void)
{
	int i;
	for (i = 0; i < ERR_MODULES_COUNT; ++i) {
		if (err_msg_arr[i]) {
			free(err_msg_arr[i]);
		}
	}
}

void err_reset(void)
{
    int i;
    for (i = 0; i < ERR_MODULES_COUNT; ++i) {
		err_msg_arr[i] = NULL;
        err_state_arr[i] = false;
    }
	err_free();
}

void err_set(enum ErrModule module, char *message)
{
	int i;
	for (i = 0; i < ERR_MODULES_COUNT; ++i) {
		if (err_state_arr[i]) {
			err_internal_error();
		}
	}

	err_state_arr[module] = true;
	err_msg_arr[module] = message;
}

void err_tok_undelimited_str(struct Location *loc)
{
	err_set(ERR_TOK, err_build_message(ERR_TOK, "Undelimited string.", loc));
}

void err_tok_undelimited_char(struct Location *loc)
{
	err_set(ERR_TOK, err_build_message(ERR_TOK, "Undelimited character.", loc));
}

void err_tok_read_failure(struct Location *loc)
{
	err_set(ERR_TOK, err_build_message(ERR_TOK, "Failed reading token.", loc));
}

void err_dom_undelimited_node(struct Location *loc)
{
	err_set(ERR_DOM, err_build_message(ERR_DOM, "Undelimited DOM node.", loc));
}

void err_dom_read_failure(struct Location *loc)
{
	err_set(ERR_DOM, err_build_message(
		ERR_DOM, "Failed reading DOM node.", loc));
}

void err_parse_bad_char_length(struct Location *loc)
{
	err_set(ERR_PARSE, err_build_message(
		ERR_PARSE, "Char literal of incorrect length.", loc));
}

void err_parse_failure(struct Location *loc)
{
	err_set(ERR_PARSE, err_build_message(
		ERR_PARSE, "Failed parsing DOM node.", loc));
}

void err_eval_bif_array_arg(
		struct Location *loc,
		int arg,
		char *func,
		char *condition)
{
	char sub_message[4 * 2048];
	sprintf(sub_message, "Argument %d of _%s_ %s.", arg, func, condition);
	err_set(ERR_EVAL, err_build_message(
		ERR_EVAL, sub_message, loc));
}

void err_eval_bif_array_common(struct Location *loc, char *issue)
{
	err_set(ERR_EVAL, err_build_message(ERR_EVAL, issue, loc));
}

bool err_state(void)
{
    int i;
    for (i = 0; i < ERR_MODULES_COUNT; ++i) {
        if (err_state_arr[i]) {
            return true;
        }
    }
    return false;
}

char *err_msg(void)
{
    int i;
    for (i = 0; i < ERR_MODULES_COUNT; ++i) {
        if (err_state_arr[i]) {
            return err_msg_arr[i];
        }
    }

    return NULL;
}

