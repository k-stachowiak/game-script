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
#include "types.h"

/*
 * This module provides the abstract expressions type. The objects representing
 * the expressions may be created here. Note that all the concrete types are
 * hidden behind an abstract expression type.
 *
 * Test status : almost tested.
 */

namespace moon
{

    namespace interpret
    {
        class environment;
    }

namespace lang
{

    struct expression
    {
        virtual ~expression() {}
        virtual moon::common::maybe<moon::lang::value> eval(const moon::interpret::environment&) const = 0;
    };

    std::unique_ptr<moon::lang::expression> expr_create_literal(moon::lang::value);
    std::unique_ptr<moon::lang::expression> expr_create_reference(const std::string&);
    std::unique_ptr<moon::lang::expression> expr_create_func_call(
            const std::string&,
            std::vector<std::unique_ptr<moon::lang::expression>>);

    // NOTE: There are more implementations of the expression class:
    // - for the bif implementation.
}
}

#endif
