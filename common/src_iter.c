#include "src_iter.h"

struct SourceLocation src_loc_normal(int line, int column)
{
	struct SourceLocation result = { SRC_LOC_NORMAL, line, column };
	return result;
}

struct SourceLocation src_loc_virtual(void)
{
	struct SourceLocation result = { SRC_LOC_VIRTUAL, -1, -1 };
	return result;
}

void si_init(struct SourceIter *si, char *first, char *last)
{
    si->first = first;
    si->current = first;
    si->last = last;
    si->loc.type = SRC_LOC_NORMAL;
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

