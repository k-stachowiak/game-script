/* Copyright (C) 2014 Krzysztof Stachowiak */

#include "common.h"

void si_init(struct SourceIter *si, char *first, char *last)
{
    si->first = first;
    si->current = first;
    si->last = last;
    si->loc.line = 0;
    si->loc.column = 0;
}

void si_adv(struct SourceIter *si)
{
    if (*(si->current) == '\n') {
        ++si->loc.line;
        si->loc.column = 0;
    } else {
        ++si->loc.column;
    }

    ++si->current;
}

void si_advn(struct SourceIter *si, int n)
{
    int i;
    for (i = 0; i < n; ++i) {
        si_adv(si);
    }
}

void si_back(struct SourceIter *si)
{
    if (*(si->current) == '\n') {
        --si->loc.line;
        si->loc.column = 0;
    } else {
        --si->loc.column;
    }

    --si->current;
}

bool si_eq(struct SourceIter *lhs, struct SourceIter *rhs)
{
    return lhs->current == rhs->current;
}

/*
 * Implementation copied from StackOverflow :
 * http://stackoverflow.com/questions/314401/how-to-read-a-line-from-the-console-in-c
 * It has been slightly modified to fit into the program, e.g. error handling.
 */
char *my_getline(bool *eof)
{
	char *line = malloc(100);
	char *linep = line;
	size_t lenmax = 100;
	size_t len = lenmax;
	int c;

	if (line == NULL) {
		printf("Allocation failed.");
		exit(1);
	}

	for (;;) {
		c = fgetc(stdin);
		if (c == EOF) {
			*eof = true;
			break;
		}

		if (--len == 0) {
			len = lenmax;
			char *linen = realloc(linep, lenmax *= 2);

			if (!linen) {
				printf("Allocation failed.\n");
				exit(1);
			}

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

	if (file) {
		fseek(file, 0, SEEK_END);
		length = ftell(file);
		fseek(file, 0, SEEK_SET);
		buffer = calloc(length + 1, 1);
		if (buffer) {
			fread(buffer, 1, length, file);
		}
		fclose(file);
	} else {
		return NULL;
	}

	return buffer;
}
