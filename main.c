/* Copyright (C) 2014 Krzysztof Stachowiak */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "error.h"
#include "repl.h"
#include "test.h"
#include "run.h"

/* TODO:
 * - Have an elegant runtime push/peek API over the basic stack API.
 * - does rt_consume_one properly releases memory upon faulure?
 * - Initialize bif values in a non-pathological way - no EVAL.
 * - Test runtime.
 * - Algorithms:
 *   - merge sort:
 *      - PREDICATE
 *      - implement the splitting phase
 * - Side effects: print
 *
 * Questions:
 * - should source locations be pushed to the stack? In debug mode?
 */

struct {
    char *module_name;
    int (*impl)(int, char*[]);
} module_map[] = {
    { "repl", repl },
    { "test", test },
    { "run", run }
};

int main(int argc, char *argv[])
{
    int error;

    /* There is an awkward memory management scheme in the error module
     * due to the lacking string building facilities in C and the convenience
     * of using them there.
     */
    atexit(err_reset);

    if (argc == 1) {
        printf("Missing command line argument.\n");
        error = 1;

    } else {

        int num_modules = sizeof(module_map)/sizeof(module_map[0]), i;
        bool found = false;

        for (i = 0; i < num_modules; ++i) {
            if (strcmp(argv[1], module_map[i].module_name) == 0) {
                found = true;
                error = module_map[i].impl(argc - 1, argv + 1);
                break;
            }
        }

        if (!found) {
            printf("Module \"%s\" not implemented.\n", argv[1]);
            error = 1;
        }
    }

    return error;
}

