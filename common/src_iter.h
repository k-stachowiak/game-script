/* Copyright (C) 2015 Krzysztof Stachowiak */

#ifndef SRC_ITER_H
#define SRC_ITER_H

#include <stdbool.h>

enum SourceLocationType {
	SRC_LOC_REGULAR,
	SRC_LOC_BIF,
	SRC_LOC_FUNC_CONTAINED
};

struct SourceLocation {
	enum SourceLocationType type;
    int line;
    int column;
};

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

#endif
