/* Copyright (C) 2014 Krzysztof Stachowiak */

#ifndef ERROR_H
#define ERROR_H

#include <stdbool.h>

struct Location;

enum ErrModule {
    ERR_TOK,
    ERR_DOM,
    ERR_PARSE,
    ERR_REPL,
    ERR_EVAL
};

#define ERR_MODULES_COUNT 5

void err_free(void);
void err_reset(void);
void err_set(enum ErrModule module, char *message);

void err_tok_undelimited_str(struct Location *loc);
void err_tok_undelimited_char(struct Location *loc);
void err_tok_read_failure(struct Location *loc);

void err_dom_undelimited_node(struct Location *loc);
void err_dom_read_failure(struct Location *loc);

void err_parse_bad_char_length(struct Location *loc); /* TODO: take length here. */
void err_parse_failure(struct Location *loc);

void err_eval_bif_array_arg(
	struct Location *loc,
	int arg,
	char *func,
	char *condition); /* TODO: provide location. */

void err_eval_bif_array_common(struct Location *loc, char *issue); /* TODO: provide location. */

bool err_state(void);
char *err_msg(void);

#endif
