/* Copyright (C) 2014 Krzysztof Stachowiak */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "error.h"
#include "repl.h"

/* TODO:
 * - Improve error messages.
 * - Assure always providing location in err_build_message.
 * - Algorithms:
 *   - merge sort:
 *      - PREDICATE
 *		- implement the splitting phase
 * - Side effects: print
 */

int main(int argc, char *argv[])
{
    int error;

	/* There is an awkward memory management scheme in the error module
	 * due to the lacking string building facilities in C and a necessity
	 * of using them there.
	 */
	atexit(err_free);

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
