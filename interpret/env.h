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

#include "common/util.h"
#include "lang/types.h"
#include "lang/expr.h"
#include "lang/func_def.h"

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
    class environment
    {
        const environment* m_parent;

        std::map<std::string, moon::lang::value> m_values;
        std::map<std::string, moon::lang::func_def> m_func_defs;
        bool m_has_vargs;
        std::vector<moon::lang::value> m_vargs;

    public:
        environment(const environment*,
                    const std::map<std::string, moon::lang::value>&,
                    std::map<std::string, moon::lang::func_def>&&,
                    bool, const std::vector<moon::lang::value>&);

        common::maybe<moon::lang::value> get_value(const std::string&) const;

        const moon::lang::func_def* get_func_def_reference(const std::string&) const;

        // Note that these serve the access for the BIF funnctions.
        // Normal code will access the vargs through the environment's symbols.
        bool has_vargs() const;
        unsigned num_vargs() const;
        const moon::lang::value& get_varg(unsigned) const;
        const std::vector<moon::lang::value>& get_vargs() const;
    };
}
}

#endif
