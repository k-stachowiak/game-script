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

#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <utility>
#include <memory>
#include <map>

namespace script
{
    class expression;

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

    struct func_def
    {
        std::vector<std::pair<std::string, value_type>> form_args;
        std::unique_ptr<expression> expr;
    };

    struct expression
    {
        virtual ~expression() {}
        virtual value eval(std::map<std::string, value>& bindings,
                           std::map<std::string, func_def>& func_defs) const = 0;
    };

    std::unique_ptr<expression> expr_create_literal(value);
    std::unique_ptr<expression> expr_create_reference(const std::string&);
    std::unique_ptr<expression> expr_create_func_call(
            const std::string&,
            std::vector<std::unique_ptr<expression>>);

}

#endif
