/* Copyright (C) 2015 Krzysztof Stachowiak */

#ifndef MOON_H
#define MOON_H

#include <stdbool.h>

enum MoonValueType {
    MN_BOOL,
    MN_CHAR,
    MN_INT,
    MN_REAL,
    MN_STRING,
    MN_ARRAY,
    MN_TUPLE,
    MN_FUNCTION
};

struct MoonValue {
    enum MoonValueType type;
    union {
        bool boolean;
        char character;
        long integer;
        double real;
        char *string;
        struct MoonValue *compound;
    } data;
    struct MoonValue *next;
};

void mn_init(void);
bool mn_exec_file(char *filename);
struct MoonValue *mn_exec_command(char *source);
void mn_dispose(struct MoonValue* value);
char *mn_error_message(void);

#endif
