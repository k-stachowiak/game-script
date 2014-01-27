/* Copyright (C) 2014 Krzysztof Stachowiak */

/*
 * This file is part of moon.
 *
 * moon is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * moon is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with moon. If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>

#include "common/sstream.h"

struct sstream *ss_init(int capacity)
{
        struct sstream *result = malloc(sizeof(*result));
        result->buffer = malloc(capacity);
        result->cap = capacity;
        result->off = 0;
        return result;
}

void ss_delete(struct sstream *ss)
{
        free(ss->buffer);
        free(ss);
}
