/* Copyright (C) 2014,2015 Krzysztof Stachowiak */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "error.h"
#include "repl.h"
#include "test.h"
#include "run.h"

/*
 * TODO:
 * - organize std.mn and load it automatically (add printf)
 * - add "at" bif, move car and cdr to std.mn; implement in terms of "at" and "slice"
 * - implement key-value store a.k.a. database
 */

struct {
    char *client_name;
    int (*impl)(int, char*[]);
} client_map[] = {
    { "repl", repl },
    { "test", test },
    { "run", run }
};

int main(int argc, char *argv[])
{
    int error;

    atexit(err_reset);

    if (argc == 1) {
        printf("Missing command line argument.\n");
        error = 1;

    } else {
        int num_clients = sizeof(client_map)/sizeof(client_map[0]), i;
        bool found = false;

        for (i = 0; i < num_clients; ++i) {
            if (strcmp(argv[1], client_map[i].client_name) == 0) {
                found = true;
                error = client_map[i].impl(argc - 1, argv + 1);
                break;
            }
        }

        if (!found) {
            printf("Client \"%s\" not implemented.\n", argv[1]);
            error = 1;
        }
    }

    return error;
}

