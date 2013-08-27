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
#include <functional>
#include <algorithm>

#include "expr.h"
#include "interpret/env.h"

namespace
{
    using namespace moon::expr;
    using namespace moon::types;
    using namespace moon::common;
    using namespace moon::interpret;

    // Literal expression.
    // -------------------

    class literal : public expression
    {
        value m_value;

    public:
        literal(const value& v)
        : m_value(v)
        {}

        maybe<value> eval(const environment&) const
        {
            return { m_value };
        }
    };

    // Reference expression.
    // ---------------------

    class reference : public expression
    {
        std::string m_symbol;

    public:
        reference(const std::string& symbol) : m_symbol(symbol) {}

        maybe<value> eval(const environment& env) const
        {
            return env.get_value(m_symbol);
        }
    };

    // Function call expression.
    // -------------------------

    class func_call : public expression
    {
        std::string m_symbol;
        std::vector<std::unique_ptr<expression>> m_actual_args;

        std::vector<maybe<value>> get_arg_values(const environment& env) const
        {
            using namespace std::placeholders;
            std::vector<maybe<value>> arg_values;
            std::transform(begin(m_actual_args),
                           end(m_actual_args),
                           std::back_inserter(arg_values),
                           std::bind(&expression::eval, _1, std::ref(env)));
            return arg_values;
        }

    public:
        func_call(const std::string& symbol,
                  std::vector<std::unique_ptr<expression>> actual_args)
        : m_symbol(symbol)
        , m_actual_args(std::move(actual_args))
        {}

        maybe<value> eval(const environment& env) const
        {
            using namespace std::placeholders;

            // 1. Evaluate arguments in the current environment.
            std::vector<maybe<value>> maybe_arg_values = get_arg_values(env);

            bool values_valid = std::all_of(
                    begin(maybe_arg_values),
                    end(maybe_arg_values),
                    std::bind(&maybe<value>::is_valid, _1));

            if (!values_valid)
            {
                return {};
            }

            std::vector<value> arg_values;
            std::transform(
                    begin(maybe_arg_values),
                    end(maybe_arg_values),
                    std::back_inserter(arg_values),
                    std::bind(&maybe<value>::get, _1));

            // 2. Find the matching function.
            const auto* fd = env.get_func_def_reference(m_symbol);
            if (!fd)
            {
                return {};
            }

            if (fd->form_args.size() != m_actual_args.size())
            {
                return {};
            }

            // 3. Derive a new environment.
            std::map<std::string, value> der_env_values;
            std::transform(
                begin(fd->form_args),
                end(fd->form_args),
                begin(arg_values),
                std::inserter(der_env_values, begin(der_env_values)),
                    [](const std::string& symbol, const value& value)
                    {
                        return std::make_pair(symbol, value);
                    });

            environment der_env(&env, der_env_values, {} );

            // 4. Execute the expression.
            return { fd->expr->eval(der_env) };
        }
    };

}

namespace moon 
{
namespace expr
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
}
