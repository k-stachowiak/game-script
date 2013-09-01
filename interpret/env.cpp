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

#include <iterator>

#include "env.h"

namespace moon 
{
namespace interpret
{

    using namespace moon::common;
    using namespace moon::lang;

    environment::environment(
            const environment* parent,
            const std::map<std::string, value>& values,
            std::map<std::string, func_def>&& func_defs,
            bool has_vargs,
            const std::vector<value>& vargs)
    : m_parent(parent)
    , m_values(values)
    , m_has_vargs(has_vargs)
    , m_vargs(vargs)
    {
        std::move(begin(func_defs), end(func_defs),
                  std::inserter(m_func_defs, begin(m_func_defs)));

        if (has_vargs)
        {
            for (unsigned arg_index = 0; arg_index < vargs.size(); ++arg_index)
            {
                // TODO: Prevent user defined symbols from starging with "#".
                std::stringstream symbol;
                symbol << varg_prefix() << arg_index;
                m_values.insert({ symbol.str(), vargs[arg_index] });
            }
            
            std::string argc_symbol = varg_prefix();
            argc_symbol.append("n");

            m_values.insert({ argc_symbol, {
                    value_type::integer,
                    static_cast<long>(vargs.size()),
                    0.0,
                    {},
                    false,
                    {} }});
        }
    }

    maybe<value> environment::get_value(const std::string& symbol) const
    {
        auto found = m_values.find(symbol);
        if (found != end(m_values))
        {
            return { found->second };
        }
        
        if (m_parent)
        {
            return m_parent->get_value(symbol);
        }
        else
        {
            return {};
        }
    }

    // TODO: Consider taking the number of the arguments into account.
    const func_def* environment::get_func_def_reference(const std::string& symbol) const
    {
        auto found = m_func_defs.find(symbol);
        if (found != end(m_func_defs))
        {
            return &(found->second);
        }

        if (m_parent)
        {
            return m_parent->get_func_def_reference(symbol);
        }
        else
        {
            return nullptr;
        }
    }

    bool environment::has_vargs() const
    {
        return m_has_vargs;
    }

    unsigned environment::num_vargs() const
    {
        if (!m_has_vargs)
        {
            throw; // TODO: handle nicely
        }

        return m_vargs.size();
    }

    const moon::lang::value& environment::get_varg(unsigned index) const
    {
        if (index >= m_vargs.size())
        {
            throw; // TODO: handle nicely;
        }

        return m_vargs[index];
    }

    const std::vector<moon::lang::value>& environment::get_vargs() const
    {
        if (!m_has_vargs)
        {
            throw; // TODO: handle nicely;
        }
        return m_vargs;
    }

}
}
