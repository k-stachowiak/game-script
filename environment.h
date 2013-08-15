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

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <map>
#include <string>
#include <memory>
#include <vector>
#include <utility>

#include "types.h"

namespace script
{

    class expression;

    struct func_def
    {
        std::vector<std::pair<std::string, value_type>> form_args;
        std::unique_ptr<expression> expr;
    };

    class environment
    {
        const environment* m_parent;

        std::map<std::string, value> m_values;

    public:
        environment(const environment*,
                    const std::map<std::string, value>& values);

        bool has_value(const std::string&) const;
        value get_value(const std::string&) const;
    };

}

#endif
