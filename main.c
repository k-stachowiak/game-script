/* Copyright (C) 2014 Krzysztof Stachowiak */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "error.h"
#include "repl.h"

/* TODO:
 * - Memory corruption!
 * - implement algorithm for visiting AST tree references.
 * - encapsulate pushing of primitives on the stack.
 * - Algorithms:
 *   - min_element
 *   - nwd, nww
 *   - merge sort
 */

int main(int argc, char *argv[])
{
    int error;

    if (argc == 1) {
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
