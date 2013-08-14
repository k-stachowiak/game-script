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

#ifndef DOM_PATTERN_H
#define DOM_PATTERN_H

#include <tuple>
#include <utility>

#include "dom.h"

namespace script {


    // Compound matching.
    // ------------------

    template<size_t I = 0, typename... Tp>
    inline typename std::enable_if<I == sizeof...(Tp), bool>::type
    match(std::tuple<Tp...> const& tup, script::node const&)
    {
        return true;
    }

    template<std::size_t I = 0, typename... Tp>
    inline typename std::enable_if<I < sizeof...(Tp), bool>::type
    match(std::tuple<Tp...> const& tup, script::node const& n)
    {
        return match(std::get<I>(tup), n.list[I]) &&
               match<I + 1, Tp...>(tup, n);
    }


    // Expect implementation.
    // ----------------------

    struct pattern_atom
    {
        friend bool match(pattern_atom const&, script::node const& n)
        {
            return (n.type == script::node_type::atom);
        }
    };
            
    struct pattern_spec_atom
    {
        std::string expected;

        friend bool match(pattern_spec_atom const& exp, script::node const& n)
        {
            if (n.type != script::node_type::atom)
            {
                return false;
            }

            return n.atom == exp.expected;
        }
    };

    struct pattern_atom_cap
    {
        std::string& reference;

        friend bool match(pattern_atom_cap const& exp, script::node const& n)
        {
            if (n.type != script::node_type::atom)
            {
                return false;
            }
            
            exp.reference = n.atom;

            return true;
        }
    };


    template<class Left, class Right, class Op>
    struct pattern_binop
    {
        Left left;
        Right right;

        friend bool match(
                pattern_binop<Left, Right, Op> const& exp,
                script::node const& n)
        {
            Op op;
            return op(match(exp.left, n), match(exp.right, n));
        }
    };

    struct pattern_list
    {
        friend bool match(pattern_list const&, script::node const& n)
        {
            return (n.type == script::node_type::list);
        }
    };

    template<class... Args>
    struct pattern_list_of
    {
        std::tuple<Args...> elements;
        friend bool match(
                pattern_list_of<Args...> const& exp,
                script::node const& n)
        {
            if (n.type != script::node_type::list)
            {
                return false;
            }

            return match(exp.elements, n);
        }
    };

}

#endif
