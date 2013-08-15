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

#include "types.h"
#include "expr.h"

namespace script
{
    struct func_def
    {
        std::vector<std::string> form_args;
        std::unique_ptr<expression> expr;

        func_def(func_def&&) = default;
    };

    class environment
    {
        const environment* m_parent;

        std::map<std::string, value> m_values;
        std::map<std::string, func_def> m_func_defs;

    public:
        environment(const environment*,
                    const std::map<std::string, value>&,
                    std::map<std::string, func_def>&&);

        bool has_value(const std::string&) const;
        value get_value(const std::string&) const;

        bool has_func_def(const std::string&) const;
        const func_def& get_func_def_reference(const std::string&) const;
    };

}

#endif
