/* Copyright (C) 2014 Krzysztof Stachowiak */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "error.h"

static char *err_message = NULL;

static void err_internal_error(void)
{
	LOG_ERROR("Error in the error system. Too embarrassed to continue.\n");
	exit(2);
}

void err_msg_init(struct ErrMessage *msg, char *module)
{
	char buffer[ERR_TEM_BUFFER_SIZE];
	int len = sprintf(buffer, "[%s] :", module);
	if (len >= (ERR_TEM_BUFFER_SIZE) - 1) {
		LOG_ERROR("Memory corruption while building error string.");
		exit(2);
	}
	msg->text = malloc_or_die(len + 1);
	memcpy(msg->text, buffer, len + 1);
}

void err_msg_init_src(struct ErrMessage *msg, char *module, struct SourceLocation *loc)
{
	char buffer[ERR_TEM_BUFFER_SIZE];
	int len;
	switch (loc->type) {
	case SRC_LOC_REGULAR:
		len = sprintf(buffer, "[%s] (%d:%d) :", module, loc->line, loc->column);
		break;
	case SRC_LOC_BIF:
		len = sprintf(buffer, "[%s] (BIF) :", module);
		break;
	case SRC_LOC_FUNC_CONTAINED:
		len = sprintf(buffer, "[%s] (FUNC) :", module);
		break;
	}
	if (len >= (ERR_TEM_BUFFER_SIZE) - 1) {
		LOG_ERROR("Memory corruption while building error string.");
		exit(2);
	}
	msg->text = malloc_or_die(len + 1);
	memcpy(msg->text, buffer, len + 1);
}

void err_msg_set(struct ErrMessage *msg)
{
    if (err_message) {
        err_internal_error();
    }
	err_message = msg->text;
}

void err_reset(void)
{
    if (err_message) {
        free(err_message);
        err_message = NULL;
    }
}

bool err_state(void)
{
    return (bool)err_message;
}

char *err_msg(void)
{
    return err_message;
}

