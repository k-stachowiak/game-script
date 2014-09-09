/* Copyright (C) 2014 Krzysztof Stachowiak */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "error.h"
#include "repl.h"

/* TODO:
 * - Have a single malloc-or-die function, and realloc, and calloc, etc.
 * - parse_func_def : use list and appending instead of silly preallocation of 2048 chars
 * - Improve error messages.
 * - Algorithms:
 *   - merge sort:
 *      - PREDICATE
 *		- implement slice
 *      - implement reverse
 *		- implement the splitting phase
 * - Side effects: print
 */

int main(int argc, char *argv[])
{
    int error;

    if (argc == 2) {
        if (strcmp(argv[1], "repl") == 0) {
            if ((error = repl())) {
                printf("REPL error: %s\n", err_msg());
            } else {
                printf("REPL terminated correctly.\n");
            }
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
