#include <stdlib.h>
#include <stdio.h>

#include "repl.h"

int main()
{
    int error;

    if ((error = repl())) {
        printf("Error: %s\n", err_msg());
    }

    return error;
}

