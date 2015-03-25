/* Copyright (C) 2014,2015 Krzysztof Stachowiak */

#ifndef MOON_H
#define MOON_H

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
};

void mn_init(void);
void mn_exec_file(char *filename);

struct MoonValue* mn_lookup(char *symbol);
struct MoonValue* mn_call(char *symbol);
void mn_dispose(struct MoonValue* value);

char *mn_error_message(void);

#endif
