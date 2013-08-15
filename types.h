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

#ifndef TYPES_H
#define TYPES_H

namespace script
{

    enum class value_type
    {
        integer,
        real,
        string
    };

    struct value
    {
        value_type type;
        long integer;
        double real;
        std::string string;
    };

    inline bool operator==(const value& lhs, const value& rhs)
    {
        if (lhs.type != rhs.type)
        {
            return false;
        }

        switch (lhs.type)
        {
            case value_type::integer:
                return lhs.integer == rhs.integer;

            case value_type::real:
                return lhs.real == rhs.real;

            case value_type::string:
                return lhs.string == rhs.string;

            default:
                // TODO: Handle nicely.
                throw;
        }
    }
}

#endif
