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

#ifndef UTIL_H
#define UTIL_H

namespace moon 
{
namespace common 
{

    template<class T>
    class maybe
    {
        bool m_valid;
        T m_value;
    public:
        maybe(const T& value) : m_valid(true) , m_value(value) {}
        maybe() : m_valid(false) {}
        bool is_valid() const { return m_valid; }
        const T& get() const { return m_value; }
    };

}
}

#endif
