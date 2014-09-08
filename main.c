/* Copyright (C) 2014 Krzysztof Stachowiak */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "error.h"
#include "repl.h"

/* TODO:
 * - implement do block - ALSO REQUIRED AS FUNCTION BODY
 *   - do block
 *	 - expect function to contain it
 * - impl malloc_or_die and replace malloc calls with it.
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
