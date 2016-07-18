/* Copyright (C) 2015 Krzysztof Stachowiak */

#include <stdio.h>

#include "memory.h"
#include "term.h"

/*
 * Implementation copied from StackOverflow :
 * http://stackoverflow.com/questions/314401/how-to-read-a-line-from-the-console-in-c
 * It has been slightly modified to fit into the program, e.g. error handling.
 */
char *my_getline(bool *eof)
{
    char *line = mem_malloc(100);
    char *linep = line;
    size_t lenmax = 100;
    size_t len = lenmax;
    int c;

    for (;;) {
        c = fgetc(stdin);
        if (c == EOF) {
            *eof = true;
            break;
        }

        if (--len == 0) {
            char *linen;
            len = lenmax;
            linen = mem_realloc(linep, lenmax *= 2);
            line = linen + (line - linep);
            linep = linen;
        }

        if ((*line++ = c) == '\n') {
            break;
        }
    }

    *line = '\0';
    return linep;
}

/*
 * Implementation copied from StackOverflow :
 * http://stackoverflow.com/questions/174531/easiest-way-to-get-files-contents-in-c
 * It has been slightly modified to fit into the program, e.g. error handling.
 */
char *my_getfile(char *filename)
{
    char *buffer = 0;
    long length;
    FILE *file = fopen(filename, "rb");

    if (!file) {
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    length = ftell(file);
    fseek(file, 0, SEEK_SET);
    buffer = mem_malloc(length + 1);

    if (fread(buffer, 1, length, file) == (size_t)length) {
        buffer[length] = '\0';
    } else {
        mem_free(buffer);
        buffer = NULL;
    }

    fclose(file);

    return buffer;
}

