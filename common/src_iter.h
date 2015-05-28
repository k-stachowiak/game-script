/* Copyright (C) 2015 Krzysztof Stachowiak */

#ifndef SRC_ITER_H
#define SRC_ITER_H

#include <stdbool.h>

enum SourceLocationType {
	SRC_LOC_NORMAL,
	SRC_LOC_VIRTUAL
};

struct SourceLocation {
    enum SourceLocationType type;
    int line;
    int column;
};

struct SourceLocation src_loc_normal(int line, int column);
struct SourceLocation src_loc_virtual(void);

struct SourceIter {
    char *first;
    char *current;
    char *last;
    struct SourceLocation loc;
};

void si_init(struct SourceIter *si, char *first, char *last);
void si_adv(struct SourceIter *si);
void si_advn(struct SourceIter *si, int n);
void si_back(struct SourceIter *si);
bool si_eq(struct SourceIter *lhs, struct SourceIter *rhs);

struct SourceIter si_find(
		struct SourceIter *begin,
		struct SourceIter *end,
		int value);

struct SourceIter si_find_if_not(struct SourceIter *begin,
		struct SourceIter *end,
		int (*pred)(int));



#endif
