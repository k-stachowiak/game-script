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

#include <sstream>
#include <iostream>

#include "env_build.h"
#include "common/except.h"
#include "common/expr.h"
#include "parse/tok.h"
#include "parse/dom.h"

namespace
{
    std::unique_ptr<script::expression> try_parse_expression(const script::node& n);

    std::unique_ptr<script::expression> try_parse_literal(const script::node& n)
    {
        if (n.type != script::node_type::atom)
        {
            return {};
        }

        if (n.atom.empty())
        {
            throw script::empty_atom_string();
        }

        // Attempt at parsing string literal.

        char delim = static_cast<char>(script::token_char::strdelim);
        unsigned length = std::distance(begin(n.atom), end(n.atom));
        if (length >= 2 && // 2 for two delimiters at the ends.
            n.atom.front() == delim &&
            n.atom.back() == delim)
        {
            return script::expr_create_literal(
                    {
                        script::value_type::string,
                        {}, {},
                        { n.atom.substr(1, length - 2) }
                    });
        }
        
        // Attempt at parsing integer literal.

        auto it = begin(n.atom);
        if ((isdigit(*it) || (*it) == '-' || (*it) == '+') &&
            all_of(it + 1, end(n.atom), isdigit))
        {
            std::stringstream ss;
            ss << n.atom;

            long integer;
            ss >> integer;

            return script::expr_create_literal(
                    {
                        script::value_type::integer,
                        integer, {}, {}
                    });
        }

        // Attempt at parsing real literal.

        {
            std::stringstream ss;
            ss << n.atom;

            double real;
            ss >> real;

            std::stringstream after_conv;
            after_conv << real;

            if (!ss.eof())
            {
                return {};
            }
            else
            {
                return script::expr_create_literal(
                    {
                        script::value_type::real,
                        {}, real, {}
                    });
            }
        }

        return {};
    }

    std::unique_ptr<script::expression> try_parse_reference(const script::node& n)
    {
        if (n.type != script::node_type::atom || isdigit(n.atom.front()))
        {
            return {};
        }

        if (n.atom.empty())
        {
            throw script::empty_atom_string();
        }

        return script::expr_create_reference(n.atom);
    }

    std::unique_ptr<script::expression> try_parse_func_call(const script::node& n)
    {
        // Assert the general structure.

        if (n.type != script::node_type::list ||
            n.list.empty())
        {
            return {};
        }

        // Parse the particular items.

        auto it = begin(n.list);

        const std::string& symbol = it->atom;
        ++it;

        std::vector<std::unique_ptr<script::expression>> args;
        std::transform(
                it, end(n.list), 
                std::back_inserter(args),
                try_parse_expression);

        return script::expr_create_func_call(symbol, std::move(args));
    }

    std::unique_ptr<script::expression> try_parse_expression(const script::node& n)
    {
        std::unique_ptr<script::expression> result;

        if (result = try_parse_literal(n))
        {
            return result;
        }

        if (result = try_parse_reference(n))
        {
            return result;
        }

        if (result = try_parse_func_call(n))
        {
            return result;
        }

        return {};
    }

}

namespace script
{

    // The API implementation.
    // =======================

}
