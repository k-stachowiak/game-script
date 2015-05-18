/* Copyright (C) 2015 Krzysztof Stachowiak */

#ifndef MOON_H
#define MOON_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

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
        uint64_t integer;
        double real;
        char *string;
        struct MoonValue *compound;
    } data;
    struct MoonValue *next;
};

typedef struct MoonValue* (*ClifHandler)(struct MoonValue *args);

void mn_init(void);

void mn_register_clif(const char *symbol, int arity, ClifHandler handler);

bool mn_exec_file(const char *filename);
struct MoonValue *mn_exec_command(const char *source);

void mn_dispose(struct MoonValue* value);

const char *mn_error_message(void);

#ifdef __cplusplus
}
#endif

#endif
