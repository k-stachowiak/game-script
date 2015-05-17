/* Copyright (C) 2014,2015 Krzysztof Stachowiak */

#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "src_iter.h"
#include "memory.h"
#include "error.h"

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
    case SRC_LOC_REGULAR:
        str_append(buffer, "[%s] (%d:%d) :", module, loc->line, loc->column);
        break;
    case SRC_LOC_BIF:
        str_append(buffer, "[%s] (BIF) :", module);
        break;
	case SRC_LOC_CLIF:
		str_append(buffer, "[%s] (CLIF) :", module);
		break;
    case SRC_LOC_FUNC_CONTAINED:
        str_append(buffer, "[%s] (FUNC) :", module);
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
}

bool err_state(void)
{
    return (bool)err_message;
}

char *err_msg(void)
{
    return err_message;
}

