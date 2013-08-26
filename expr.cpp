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

        script::maybe<script::value> eval(const script::environment&) const
        {
            return { m_value };
        }

        script::maybe<script::value_type> get_type(const script::environment&) const
        {
            return { m_value.type };
        }
    };

    // Reference expression.
    // ---------------------

    class reference : public script::expression
    {
        std::string m_symbol;

    public:
        reference(const std::string& symbol) : m_symbol(symbol) {}

        script::maybe<script::value> eval(const script::environment& env) const
        {
            return env.get_value(m_symbol);
        }

        // TODO: Is get_type needed if dynamic evaluation?
        script::maybe<script::value_type> get_type(const script::environment& env) const
        {
            auto maybe_value = env.get_value(m_symbol);
            if (!maybe_value.is_valid())
            {
                return {};
            }
            else
            {
                return maybe_value.get().type;
            }
        }
    };

    // Function call expression.
    // -------------------------

    class func_call : public script::expression
    {
        std::string m_symbol;
        std::vector<std::unique_ptr<script::expression>> m_actual_args;

        std::vector<script::maybe<script::value_type>> get_arg_types(const script::environment& env) const
        {
            using namespace std::placeholders;
            std::vector<script::maybe<script::value_type>> arg_types;
            std::transform(begin(m_actual_args),
                           end(m_actual_args),
                           std::back_inserter(arg_types),
                           std::bind(&script::expression::get_type, _1, std::ref(env)));
            return arg_types;
        }

        std::vector<script::maybe<script::value>> get_arg_values(const script::environment& env) const
        {
            using namespace std::placeholders;
            std::vector<script::maybe<script::value>> arg_values;
            std::transform(begin(m_actual_args),
                           end(m_actual_args),
                           std::back_inserter(arg_values),
                           std::bind(&script::expression::eval, _1, std::ref(env)));
            return arg_values;
        }

    public:
        func_call(const std::string& symbol,
                  std::vector<std::unique_ptr<script::expression>> actual_args)
        : m_symbol(symbol)
        , m_actual_args(std::move(actual_args))
        {}

        script::maybe<script::value> eval(const script::environment& env) const
        {
            using namespace std::placeholders;

            // 1. Determine the types.
            std::vector<script::maybe<script::value_type>> maybe_arg_types = get_arg_types(env);

            bool types_valid = std::all_of(
                    begin(maybe_arg_types),
                    end(maybe_arg_types),
                    std::bind(&script::maybe<script::value_type>::is_valid, _1));

            if (!types_valid)
            {
                return {};
            }

            std::vector<script::value_type> arg_types;
            std::transform(
                    begin(maybe_arg_types),
                    end(maybe_arg_types),
                    std::back_inserter(arg_types),
                    std::bind(&script::maybe<script::value_type>::get, _1));

            // 2. Evaluate arguments in the current environment.
            std::vector<script::maybe<script::value>> maybe_arg_values = get_arg_values(env);

            bool values_valid = std::all_of(
                    begin(maybe_arg_values),
                    end(maybe_arg_values),
                    std::bind(&script::maybe<script::value>::is_valid, _1));

            if (!values_valid)
            {
                return {};
            }

            std::vector<script::value> arg_values;
            std::transform(
                    begin(maybe_arg_values),
                    end(maybe_arg_values),
                    std::back_inserter(arg_values),
                    std::bind(&script::maybe<script::value>::get, _1));

            // 3. Find the matching function.
            const auto* fd = env.get_func_def_reference(m_symbol, arg_types);
            if (!fd)
            {
                return {};
            }

            if (fd->form_args.size() != m_actual_args.size())
            {
                return {};
            }

            // 4. Derive a new environment.
            std::map<std::string, script::value> der_env_values;
            std::transform(
                begin(fd->form_args),
                end(fd->form_args),
                begin(arg_values),
                std::inserter(der_env_values, begin(der_env_values)),
                    [](const std::string& symbol, const script::value& value)
                    {
                        return std::make_pair(symbol, value);
                    });

            script::environment der_env(&env, der_env_values, {} );

            // 5. Execute the expression.
            return { fd->expr->eval(der_env) };
        }

        script::maybe<script::value_type> get_type(const script::environment& env) const
        {
            using namespace std::placeholders;

            std::vector<script::maybe<script::value_type>> maybe_arg_types = get_arg_types(env);
            
            bool types_valid = std::all_of(
                    begin(maybe_arg_types),
                    end(maybe_arg_types),
                    std::bind(&script::maybe<script::value_type>::is_valid, _1));

            if (!types_valid)
            {
                return {};
            }

            std::vector<script::value_type> arg_types;
            std::transform(
                    begin(maybe_arg_types),
                    end(maybe_arg_types),
                    std::back_inserter(arg_types),
                    std::bind(&script::maybe<script::value_type>::get, _1));

            const auto* fd = env.get_func_def_reference(m_symbol, arg_types);
            if (!fd)
            {
                return {};
            }
            
            return { fd->expr->get_type(env) };
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
