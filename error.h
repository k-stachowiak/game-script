#ifndef ERROR_H
#define ERROR_H

#include <stdbool.h>

enum ErrModule {
    ERR_TOK,
    ERR_DOM,
    ERR_PARSE,
    ERR_REPL,
    ERR_MAX
};

void err_reset(void);
void err_set(enum ErrModule module, char *message);

bool err_state(void);
char *err_msg(void);

#endif
