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

#ifndef EXPR_H
#define EXPR_H

#include <string>
#include <vector>
#include <memory>
#include <functional>

#include "types.h"

namespace script
{
    class environment;

    struct expression
    {
        virtual ~expression() {}
        virtual value eval(const environment& env) const = 0;
    };

    std::unique_ptr<expression> expr_create_literal(value);
    std::unique_ptr<expression> expr_create_reference(const std::string&);
    std::unique_ptr<expression> expr_create_func_call(
            const std::string&,
            std::vector<std::unique_ptr<expression>>);
}

#endif
