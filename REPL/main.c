/* Copyright (C) 2015 Krzysztof Stachowiak */

#include <stdbool.h>
#include <stdio.h>

#include "term.h"
#include "memory.h"
#include "moon.h"

static bool quit_request = false;
static char *stdfilename = "std.mn";
static char *prompt = "> ";
static char *banner =
    "Moon language REPL\n"
    "Copyright (C) 2014-2015 Krzysztof Stachowiak\n";

static void repl_register_clifs(void)
{
}

static char *repl_read(void)
{
    bool eof_flag;
    char *line;

    printf(prompt);
    line = my_getline(&eof_flag);

    if (eof_flag) {
        mem_free(line);
        return NULL;
    }

    return line;
}

static struct MoonValue *repl_evaluate(char *line)
{
    return mn_exec_command(line);
}

static struct MoonValue *repl_print(struct MoonValue *value)
{
}

int main()
{
    printf(banner);

    mn_init();
    mn_exec_file(stdfilename);
    mn_register_clif("debug", 0, repl_clif_dbg);
    mn_register_clif("quit", 0, repl_clif_quit);
    mn_register_clif("peek", 1, repl_clif_quit);

    while (!quit_request) {

        char *line;
        struct MoonValue *value;

        if (!(line = repl_read())) {
            return 0;
        }

        if (!(value = repl_evaluate(line))) {
            printf(mn_error_message);
            return 1;
        }

        repl_print(value);
    }

    return 0;
}
