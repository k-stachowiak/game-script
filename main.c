/* Copyright (C) 2014 Krzysztof Stachowiak */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "error.h"
#include "repl.h"

/* TODO:
 * - Encapsulate pushing of primitives on the stack:
 *   - find all clients of generic push,
 *   - then find all the clients of the clients and so on,
 *   - pick an elegant way to generalize them.
 * - Algorithms:
 *   - min_element
 *   - nwd, nww
 *   - merge sort
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
