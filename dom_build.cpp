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

#include <iterator>

#include "dom_build.h"

namespace
{

    inline bool is_atom(const std::string& tok)
    {
        return tok.size() != 1 || (tok[0] != '(' && tok[0] != ')');
    }

    inline bool is_lopen(const std::string& tok)
    {
        return tok.size() == 1 && tok[0] == '(';
    }

    inline bool is_lclose(const std::string& tok)
    {
        return tok.size() == 1 && tok[0] == ')';
    }

}

namespace script
{

    template<class InIter, class OutIter>
    InIter read_list(InIter first, InIter last, OutIter out)
    {
        auto current = first;

        while (current != last)
        {
            // Atom case.
            if (is_atom(*current))
                *out++ = { node_type::atom, *current++, {} };

            // SubList case.
            else if (is_lopen(*current))
            {
                std::vector<node> sub_list;
                current = read_list(++current, last, std::back_inserter(sub_list));
                *out++ = { node_type::list, {}, sub_list };
            }

            // End of list case.
            else if (is_lclose(*current))
            {
                ++current;
                return current;
            }
        }

        // List must always be terminated with a closing parenthesis.
        exit(1);
    }

    node build_dom_tree(std::vector<std::string>& tokens)
    {
        // Append synthetic list terminating parenthesis.
        // The opening parenthesis may be assumed implicitly.
        tokens.push_back(")");

        // Read the top level list.
        std::vector<node> top_nodes;
        read_list(begin(tokens), end(tokens), std::back_inserter(top_nodes));

        // Remove the synthetic closing parenthesis.
        tokens.pop_back();

        return { node_type::list, {}, top_nodes };
    }

}
