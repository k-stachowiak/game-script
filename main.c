/* Copyright (C) 2014,2015 Krzysztof Stachowiak */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "error.h"
#include "repl.h"
#include "test.h"
#include "test2.h"
#include "run.h"

/*
 * TODO:
 * =====
 * - Thorough tesets:
 *  - Front-end
 *  - Runtime:
 *      - Operation of all AST elements
 *      - BIF correctness
 * - Algorithms:
 *  - Enable predicates in the algorithms
 * - Improve capturing algorithm so that captured functions are bound to the local symmap.
 * - print BIF
 * - File type
 *
 * Questions:
 * ==========
 * - should source locations be pushed to the stack? In debug mode?
 */

struct {
    char *client_name;
    int (*impl)(int, char*[]);
} client_map[] = {
    { "repl", repl },
    { "test", test },
    { "test2", test2 },
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

