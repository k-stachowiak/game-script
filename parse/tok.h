/* Copyright (C) 2013 Krzysztof Stachowiak */

/*
 * This file is part of gme-script.
 *
 * gme-script is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * gme-script is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with gme-script. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TOK_H
#define TOK_H

namespace script
{
    enum class token_char : char
    {
        lopen = '(',        // list opening token
        lclose = ')',       // list closing token
        comment = ';',      // comment initializing token
        strdelim = '"',     // string delimiter
        strescape = '\\'    // stirng delimiter escape
    };
}

#endif
