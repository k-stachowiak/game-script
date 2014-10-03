/* Copyright (C) 2014 Krzysztof Stachowiak */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "error.h"
#include "repl.h"
#include "test.h"

/* TODO:
 * - Refactor printing to new API
 * - ERROR: evaluate string -> results in printing a boolean.
 * - Algorithms:
 *   - merge sort:
 *      - PREDICATE
 *      - implement the splitting phase
 * - Side effects: print
 *
 * Questions:
 * - should source locations be pushed to the stack? In debug mode?
 */

int main(int argc, char *argv[])
{
    int error;

    /* There is an awkward memory management scheme in the error module
     * due to the lacking string building facilities in C and a necessity
     * of using them there.
     */
    atexit(err_reset);

    if (argc == 2) {
        if (strcmp(argv[1], "repl") == 0) {
            error = repl();

		} else if (strcmp(argv[1], "test") == 0) {
			error = test();
			
        } else {
            printf("Incorrect args.\n");
            error = 1;
        }
    } else {
        printf("Missing command line argument.\n");
        error = 1;
    }

    return error;
}
