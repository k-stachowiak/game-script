/* Copyright (C) 2014 Krzysztof Stachowiak */

#include "common.h"

void si_init(struct SourceIter *si, char *first, char *last)
{
    si->first = first;
    si->current = first;
    si->last = last;
    si->loc.type = SRC_LOC_REGULAR;
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
	char *line = malloc_or_die(100);
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
			linen = realloc_or_die(linep, lenmax *= 2);
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
		buffer = calloc_or_die(length + 1, 1);
		fread(buffer, 1, length, file);
		fclose(file);
	} else {
		return NULL;
	}

	return buffer;
}

void *malloc_or_die(size_t size)
{
	void *result = malloc(size);
	if (!result) {
		LOG_ERROR("Allocation failure.");
		exit(1);
	}
	LOG_DEBUG("*malloc* (%p)", result);
	return result;
}

void *calloc_or_die(size_t count, size_t size)
{
	void *result = calloc(count, size);
	if (!result) {
		LOG_ERROR("Callocation failure.");
		exit(1);
	}
	LOG_DEBUG("*calloc* (%p)", result);
	return result;
}

void *realloc_or_die(void *old, size_t size)
{
	void *result = realloc(old, size);
	if (!result) {
		LOG_ERROR("Reallocation failure.");
		exit(1);
	}
	LOG_DEBUG("*realloc* (%p)", result);
	return result;
}

void free_or_die(void *ptr)
{
	LOG_DEBUG("*free* (%p)", ptr);
	free(ptr);
}
