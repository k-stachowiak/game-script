/* Copyright (C) 2014 Krzysztof Stachowiak */

#ifndef ERROR_H
#define ERROR_H

#include <stdbool.h>
#include <string.h>

struct SourceLocation;

enum ErrModule {
	ERR_NEU,
    ERR_TOK,
    ERR_DOM,
    ERR_PARSE,
    ERR_REPL,
    ERR_EVAL
};

#define ERR_MODULES_COUNT 6

#define ERR_TEM_BUFFER_SIZE 2048

struct ErrMessage {
	char *text;
};

/* Common API. */
void err_free(void);
void err_reset(void);
bool err_state(void);
char *err_msg(void);

/* NEU API. */
void err_msg_init(struct ErrMessage *msg, char *module, struct SourceLocation *loc);
void err_set_msg(struct ErrMessage *msg);

#define err_msg_append(MSG, FORMAT, ...) \
	do { \
		char buffer[ERR_TEM_BUFFER_SIZE]; \
		int old_len, new_len; \
		char *new_text; \
		new_len = sprintf(buffer, " " FORMAT, ##__VA_ARGS__); \
		if (new_len >= (ERR_TEM_BUFFER_SIZE) - 1) { \
			LOG_ERROR("Memory corruption while building error string."); \
			exit(2); \
		} \
		old_len = strlen((MSG)->text); \
		new_text = malloc_or_die(old_len + new_len + 1); \
		memcpy(new_text, (MSG)->text, old_len); \
		memcpy(new_text + old_len, buffer, new_len + 1); \
		free((MSG)->text); \
		(MSG)->text = new_text; \
	} while(0)

/* Alt API. */
void err_set(enum ErrModule module, char *message);


#endif
