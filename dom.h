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

#ifndef DOM_H
#define DOM_H

#include <string>
#include <vector>

namespace script
{

    enum class node_type { atom, list };

    struct node
    {
        node_type type;
        std::string atom;
        std::vector<node> list;
    };

    inline bool operator==(const node& lhs, const node& rhs)
    {
        return lhs.type == rhs.type &&
               lhs.atom == rhs.atom &&
               lhs.list.size() == rhs.list.size() &&
               std::equal(begin(lhs.list), end(lhs.list), begin(rhs.list));
    }

}

#endif
