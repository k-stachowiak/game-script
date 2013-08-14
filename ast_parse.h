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

#ifndef AST_PARSE_H
#define AST_PARSE_H

namespace script
{
/*
    struct alt_parse
    {
        std::function<std::unique_ptr<script::expression> (const script::node&)> par1;
        std::function<std::unique_ptr<script::expression> (const script::node&)> par2;

        std::unique_ptr<script::expression> operator()(const script::node& n)
        {
            auto expr1 = par1(n);
            if (expr1)
            {
                return expr1;
            }

            return par2(n);
        }
    };

    std::unique_ptr<script::expression> try_parse_func_call(const script::node& n)
    {
        // General assertion.

        if (n.type != script::node_type::list)
            return {};

        // Read the function call symbol.

        auto current = b.list.begin();
        auto last = b.list.last();

        if (current->type != script::node_type::atom)
            return {};

        std::string func_call_symbol = (current++)->atom;

        // Read the actual parameters.

        std::vector<std::unique_ptr<script::expression>> actual_args;

        std::transform(
                current, last,
                std::back_inserter(actual_args),
                alt_parse {
                    try_parse_ref,
                    try_parse_func_call });

        if (success)
        {
            return script::expr_create_func_call(
                    func_call_symbol,
                    actual_args);
        }
        else
        {
            return {};
        }
    
        return {};
    }
*/
}

#endif
