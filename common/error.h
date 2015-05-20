/* Copyright (C) 2014,2015 Krzysztof Stachowiak */

#ifndef ERROR_H
#define ERROR_H

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "log.h"
#include "collection.h"
#include "memory.h"
#include "strbuild.h"
#include "src_iter.h"

struct ErrFrame {
	char *module;
	char *message;
	struct SourceLocation src_loc;
	struct ErrFrame *next;
};

extern struct ErrFrame *err_stack;
extern struct ErrFrame *err_stack_end;

#define err_push(MODULE, SRC_LOC, FORMAT, ...) \
	do { \
		struct ErrFrame *_frame_ = mem_malloc(sizeof(*_frame_)); \
		_frame_->module = (MODULE); \
		_frame_->message = NULL; \
		_frame_->src_loc = (SRC_LOC); \
		_frame_->next = NULL; \
		str_append(_frame_->message, FORMAT, ##__VA_ARGS__); \
		LIST_APPEND(_frame_, &err_stack, &err_stack_end); \
	} while(0)

struct ErrMessage {
    char *text;
};

void err_reset(void);
bool err_state(void);
char *err_msg(void);
void err_report(void);

void err_msg_init(struct ErrMessage *msg, char *module);
void err_msg_init_src(struct ErrMessage *msg, char *module, struct SourceLocation *loc);
void err_msg_set(struct ErrMessage *msg);

#define err_msg_append(MSG, FORMAT, ...) \
    do { \
        str_append((MSG)->text, " "); \
        str_append((MSG)->text, FORMAT, ##__VA_ARGS__); \
    } while (0)

#endif
