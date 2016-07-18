/* Copyright (C) 2016 Krzysztof Stachowiak */

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "strbuild.h"

char *indentation = ".   ";

static bool char_in(int c, char *array)
{
    while (*array) {
        if (c == *array++) {
            return true;
        }
    }
    return false;
}

static int breaks_token(int c)
{
    return isspace(c) || char_in(c, ")}]");
}

static char *find_if_not(char* ptr, int (*pred)(int))
{
    /* 0. Overview:
     * This is a simple algorithm that will increment a pointer to char until
     * it points to a NUL character or a provided predicate is not satisfied.
     */
    while (*ptr && pred(*ptr)) {
        ptr++;
    }

    return ptr;
}

static char *find_if(char* ptr, int (*pred)(int))
{
    /* 0. Overview:
     * This is a simple algorithm that will increment a pointer to char until
     * it points to a NUL character or a provided predicate is satisfied.
     */
    while (*ptr && !pred(*ptr)) {
        ptr++;
    }

    return ptr;
}

static void pretty_print_rec(char **input, char **output, int level)
{
    /* 0. Overview:
     * This function prints a list of a symbolic expressions. In case of
     * encountering a sub-list a recursive call with an incremented level value
     * is made. The input consists of two pointers to pointers that represent
     * the input and the output strings respectively. This way of passing them
     * allows for the use of the recursion. Additionally the level argument is
     * used to maintain the information about the recursion level.
     *
     * The expected output is:
     * <opening-delimiter>
     *     <item>
     *     <item>
     *     <opening-delimieter>
     *         <item>
     *         ...
     *     <closing-delimiter>
     *     ...
     * <closing-delimiter>
     */

    int i;

    /* 1. Iterate over an assumed list of symbolic expressions */
    while (**input) {
        /* 1.1. Skip whitespaces */
        *input = find_if_not(*input, isspace);

        /* 1.2. If reached end of string, return.
         * Note that this is the only place in which the end of file is
         * acceptable. Otherwise it indicates an error in the input
         * structure.
         */
        if (**input == '\0') {
            return;
        }

        /* 1.3. If not at the end then we definitely print something, therefore
         * indent now.
         */
        for (i = 0; i < level; ++i) {
            str_append(*output, "%s", indentation);
        }

        /* 1.4. Dispatch based on the printing variant. */
        if (char_in(**input, ")]}")) {
            /* 1.4.a) If reacned end of list, print and return */
            str_append(*output, "%c\n", **input);
            (*input)++;
            return;

        } else if (char_in(**input, "([{")) {
            /* 1.4.b) If reached begining of sub-list, print and recur */
            str_append(*output, "%c\n", **input);
            (*input)++;
            pretty_print_rec(input, output, level + 1);

        } else {
            /* 1.4.c) Otherwise print until whitespace */
            char *temp = find_if(*input, breaks_token);
            str_append_range(*output, *input, temp);
            str_append(*output, "\n");
            *input = temp;
        }
    }
}

char *sexpr_prettyprint(char *input)
{
    /* 0. Overview:
     * This function is the entry point for the recursive pretty printer for
     * the symbolic expression string. The details of its operation are
     * provided inside the recursive function called here.
     */
    char *output = NULL;
    pretty_print_rec(&input, &output, 0);
    return output;
}
