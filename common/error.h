/* Copyright (C) 2014,2015 Krzysztof Stachowiak */

#ifndef ERROR_H
#define ERROR_H

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "log.h"
#include "memory.h"
#include "strbuild.h"

struct SourceLocation;

struct ErrMessage {
    char *text;
};

void err_reset(void);
bool err_state(void);
char *err_msg(void);

void err_msg_init(struct ErrMessage *msg, char *module);
void err_msg_init_src(struct ErrMessage *msg, char *module, struct SourceLocation *loc);
void err_msg_set(struct ErrMessage *msg);

#define err_msg_append(MSG, FORMAT, ...) \
    do { \
        str_append((MSG)->text, " "); \
        str_append((MSG)->text, FORMAT, ##__VA_ARGS__); \
    } while (0)

#endif
