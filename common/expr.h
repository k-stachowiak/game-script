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

#include "common/util.h"
#include "common/types.h"

/*
 * This module provides the abstract expressions type. The objects representing
 * the expressions may be created here. Note that all the concrete types are
 * hidden behind an abstract expression type.
 *
 * Test status : almost tested.
 */

namespace script
{
    class environment;

    struct expression
    {
        virtual ~expression() {}
        virtual maybe<value> eval(const environment&) const = 0;
        virtual maybe<value_type> get_type(const environment&) const = 0;
    };

    std::unique_ptr<expression> expr_create_literal(value);
    std::unique_ptr<expression> expr_create_reference(const std::string&);
    std::unique_ptr<expression> expr_create_func_call(
            const std::string&,
            std::vector<std::unique_ptr<expression>>);

    // NOTE: There are more implementations of the expression class:
    // - for the bif implementation.
}

#endif
