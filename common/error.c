/* Copyright (C) 2014,2015 Krzysztof Stachowiak */

#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "src_iter.h"
#include "memory.h"
#include "error.h"

struct ErrFrame *err_stack = NULL;
struct ErrFrame *err_stack_end = NULL;

static char *err_message = NULL;

static void err_internal_error(void)
{
    LOG_ERROR("Error in the error system. Too embarrassed to continue.\n");
    exit(2);
}

void err_msg_init(struct ErrMessage *msg, char *module)
{
    char *buffer = NULL;
    str_append(buffer, "[%s] :", module);
    msg->text = buffer;
}

void err_msg_init_src(struct ErrMessage *msg, char *module, struct SourceLocation *loc)
{
    char *buffer = NULL;
    switch (loc->type) {
    case SRC_LOC_NORMAL:
        str_append(buffer, "[%s] (%d:%d) :", module, loc->line, loc->column);
        break;
    case SRC_LOC_VIRTUAL:
        str_append(buffer, "[%s] (-:-) :", module);
        break;
    }
    msg->text = buffer;
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
        mem_free(err_message);
        err_message = NULL;
    }

	err_stack_end = NULL;
	while (err_stack) {
		struct ErrFrame *next = err_stack->next;
		mem_free(err_stack->message);
		mem_free(err_stack);
		err_stack = next;
	}
}

bool err_state(void)
{
    return (bool)err_message || (bool)err_stack;
}

char *err_msg(void)
{
    return err_message;
}

void err_report(void)
{
	struct ErrFrame *frame;
	while (frame) {
		char *message = NULL;
		str_append(message, "[%s] ", frame->module);
		if (frame->src_loc.type == SRC_LOC_NORMAL) {
			str_append(message, "(%d,%d) ", frame->src_loc.line, frame->src_loc.column);
		}
		str_append(message, ": %s\n", frame->message);
		printf("%s", message);
		mem_free(message);
		frame = frame->next;
	}
}

