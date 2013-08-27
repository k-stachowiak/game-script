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

#ifndef ENV_H
#define ENV_H

#include <map>
#include <string>
#include <memory>
#include <vector>
#include <utility>

#include "common/types.h"
#include "common/util.h"
#include "common/expr.h"

/*
 * The environment is a container of symbols and the related expressions.
 * The two kind of items that may be stored in the environment are values
 * and functions.
 * Note that the functions are returned by pointer.
 * - They cannot be passed by value (or maybe), because they depend on
 *     the unique_ptr which may only be moved.
 * - They cannot be passed by reference, because the result may be
 *     "invalid", which is best represented by a null pointer in this case.
 *
 * Test status : Tested.
 */

namespace moon 
{
namespace interpret
{
    // Function definition - should this be here? (TODO)
    struct func_def
    {
        std::vector<std::string> form_args;
        std::unique_ptr<moon::expr::expression> expr;

        func_def(func_def&&) = default;
    };

    class environment
    {
        const environment* m_parent;

        std::map<std::string, moon::types::value> m_values;
        std::map<std::string, func_def> m_func_defs;

        bool signature_matches(
                const func_def& fd,
                std::vector<moon::types::value_type> signature);

    public:
        environment(const environment*,
                    const std::map<std::string, moon::types::value>&,
                    std::map<std::string, func_def>&&);

        common::maybe<moon::types::value> get_value(const std::string&) const;

        const func_def* get_func_def_reference(
                const std::string&,
                const std::vector<moon::types::value_type>&) const;
    };
}
}

#endif
