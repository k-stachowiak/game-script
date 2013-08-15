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

#include <utility>
#include <algorithm>

#include "expr.h"
#include "env.h"

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
                  std::vector<std::unique_ptr<script::expression>> actual_args)
        : m_symbol(symbol)
        , m_actual_args(std::move(actual_args))
        {}

        script::value eval(const script::environment& env) const
        {
            // 1. Evaluate arguments in the current environment.
            std::vector<script::value> actual_values;
            std::transform(
                begin(m_actual_args), end(m_actual_args),
                std::back_inserter(actual_values),
                    [&env](const std::unique_ptr<script::expression>& expr)
                    {
                        return expr->eval(env);
                    });

            // 2. Determine the called function.
            if (!env.has_func_def(m_symbol))
            {
                // TODO: Handle nicely.
                throw;
            }

            const auto& func = env.get_func_def_reference(m_symbol);

            if (!func.form_args.size() == actual_values.size())
            {
                // TODO: Handle nicely.
                throw;
            }

            // 3. Derive a new environment.
            std::map<std::string, script::value> der_env_values;
            std::transform(
                begin(func.form_args), end(func.form_args),
                begin(actual_values),
                std::inserter(der_env_values, begin(der_env_values)),
                    [](const std::string& symbol, const script::value& value)
                    {
                        return std::make_pair(symbol, value);
                    });

            std::map<std::string, script::func_def> empty_func_defs;
            script::environment der_env(&env, der_env_values, std::move(empty_func_defs));

            // 4. Execute the expression.
            return func.expr->eval(der_env);
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
