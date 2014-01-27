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

#ifndef SSTREAM_H
#define SSTREAM_H

#include <stdio.h>

struct sstream
{
        char *buffer;
        int cap;
        int off;
};

#define ss_printf(SS, FORMAT, ...) \
do { \
        ss->off += snprintf( \
                SS->buffer + SS->off, \
                SS->cap - SS->off, \
                FORMAT, \
                ##__VA_ARGS__); \
        if (ss->off > ss->cap) ss->off = ss->cap; \
} while(0)

struct sstream *ss_init(int capacity);
void ss_delete(struct sstream* ss);

#endif
