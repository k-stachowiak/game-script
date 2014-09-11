/* Copyright (C) 2014 Krzysztof Stachowiak */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "error.h"

static bool err_state_arr[ERR_MODULES_COUNT];
static char *err_msg_arr[ERR_MODULES_COUNT];

static void err_internal_error(void)
{
	LOG_ERROR("Error in the error system. Too embarrassed to continue.\n");
	exit(2);
}

void err_msg_init(struct ErrMessage *msg, char *module, struct Location *loc)
{
	/* TODO: Get rid of this. */
	struct Location dummy_loc = { -6, -66 };

	if (!loc) {
		loc = &dummy_loc;
	}

	char buffer[ERR_TEM_BUFFER_SIZE];
	int len = sprintf(buffer, "[%s] (%d:%d) :", module, loc->line, loc->column);
	if (len >= (ERR_TEM_BUFFER_SIZE) - 1) {
		LOG_ERROR("Memory corruption while building error string.");
		exit(2);
	}
	msg->text = malloc_or_die(len + 1);
	memcpy(msg->text, buffer, len + 1);
}

void err_set_msg(struct ErrMessage *msg)
{
	err_set(ERR_NEU, msg->text);
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

