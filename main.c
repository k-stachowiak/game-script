/* Copyright (C) 2014 Krzysztof Stachowiak */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "error.h"
#include "repl.h"
#include "autotest.h"

int main(int argc, char *argv[])
{
    int error;

    if (argc > 1) {
        if (strcmp(argv[1], "repl") == 0) {
            if ((error = repl())) {
                printf("REPL error: %s\n", err_msg());
            } else {
                printf("REPL terminated correctly.\n");
            }
        } else {
            printf("Incorrect args.\n");
        }
    } else {
        if ((error = autotest())) {
            printf("Autotest error.\n");
        } else {
            printf("Autotest terminated correctly.\n");
        }
    }

    return error;
}
