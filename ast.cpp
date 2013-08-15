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

#include "ast.h"
#include "environment.h"

namespace
{

    // Literal expression.
    // -------------------

    class literal : public script::expression
    {
        script::value m_value;

    public:
        literal(const script::value& v)
        : m_value(v)
        {}

        script::value eval(const script::environment&) const
        {
            return m_value;
        }
    };

    // Reference expression.
    // ---------------------

    class reference : public script::expression
    {
        std::string m_symbol;

    public:
        reference(const std::string& symbol) : m_symbol(symbol) {}

        script::value eval(const script::environment& env) const
        {
            if (!env.has_value(m_symbol))
            {
                // TODO: Signal error here.
                throw;
            }

            return env.get_value(m_symbol);
        }
    };

    // Function call expression.
    // -------------------------

    class func_call : public script::expression
    {
        std::string m_symbol;
        std::vector<std::unique_ptr<script::expression>> m_actual_args;

    public:
        func_call(const std::string& symbol,
                  std::vector<std::unique_ptr<expression>> actual_args)
        : m_symbol(symbol)
        , m_actual_args(std::move(actual_args))
        {}

        script::value eval(const script::environment& env) const
        {
            // TODO: Implement.
            throw;
        }
    };

}

namespace script
{
    
    // Concrete expression type constructors.
    // --------------------------------------

    std::unique_ptr<expression> expr_create_literal(value v)
    {
        return std::unique_ptr<expression>(new literal(v));
    }

    std::unique_ptr<expression> expr_create_reference(const std::string& symbol)
    {
        return std::unique_ptr<expression>(new reference(symbol));
    }

    std::unique_ptr<expression> expr_create_func_call(
            const std::string& symbol,
            std::vector<std::unique_ptr<expression>> actual_args)
    {
        return std::unique_ptr<expression>(new func_call(symbol, std::move(actual_args)));
    }
}
