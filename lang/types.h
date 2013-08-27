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

#include <string>
#include "common/util.h"
#include "common/except.h"

namespace moon 
{
namespace lang
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
                throw moon::except::type_mismatch();
        }
    }

    template<typename To>
    struct convert_to;

    template<> struct convert_to<long>
    {
        moon::common::maybe<long> operator()(const value& val)
        {
            if (val.type != value_type::integer)
            {
                return {};
            }
            return { val.integer };
        }
    };

    template<> struct convert_to<double>
    {
        moon::common::maybe<double> operator()(const value& val)
        {
            if (val.type != value_type::real)
            {
                return {};
            }
            return { val.real };
        }
    };

    template<> struct convert_to<std::string>
    {
        moon::common::maybe<std::string> operator()(const value& val)
        {
            if (val.type != value_type::string)
            {
                return {};
            }
            return { val.string };
        }
    };

    inline value convert_from(long val)
    {
        return value { value_type::integer, val, 0.0, {} };
    }

    inline value convert_from(double val)
    {
        return value { value_type::real, 0, val, {} };
    }

    inline value convert_from(const std::string& val)
    {
        return value { value_type::string, 0, 0.0, val };
    }

    template<typename To> struct type_of;
    template<> struct type_of<int> { static const value_type type; };
    template<> struct type_of<double> { static const value_type type; };
    template<> struct type_of<std::string> { static const value_type type; };

}
}

#endif
