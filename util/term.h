/* Copyright (C) 2014-2016 Krzysztof Stachowiak */

#ifndef TERM_H
#define TERM_H

#include <stdbool.h>
#include <stdio.h>

char *my_getline(FILE *file, bool *eof);
char *my_getfile(char *filename);

#endif
