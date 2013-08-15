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

#include "environment.h"

namespace script
{

    environment::environment(
            const environment* parent,
            const std::map<std::string, value>& values)
    : m_parent(parent)
    , m_values(values)
    {}

    bool environment::has_value(const std::string& symbol) const
    {
        if (m_values.find(symbol) != end(m_values))
        {
            return true;
        }

        if (m_parent)
        {
            return m_parent->has_value(symbol);
        }
        else
        {
            return false;
        }
    }

    value environment::get_value(const std::string& symbol) const
    {
        auto found = m_values.find(symbol);
        if (found != end(m_values))
        {
            return found->second;
        }
        
        if (m_parent)
        {
            return m_parent->get_value(symbol);
        }
        else
        {
            // TODO: handle elegantly.
            throw;
        }
    }

}
