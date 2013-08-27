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

#include <array>
#include <utility>
#include <tuple>

#include "env_bif.h"
#include "common/expr.h"
#include "common/util.h"

namespace // module implementation.
{
    using namespace moon::expr;
    using namespace moon::common;
    using namespace moon::interpret;
    using namespace moon::types;

    template<class Ret, class Arg1, class Arg2>
    struct bif_expression_2 : public expression
    {
        std::array<std::string, 2> m_actual_args;
        std::function<Ret (Arg1, Arg2)> m_func;

        bif_expression_2(const std::array<std::string, 2>& actual_args,
                         const std::function<Ret (Arg1, Arg2)> func)
        : m_actual_args(actual_args)
        , m_func(func)
        {}

        maybe<value> eval(const environment& env) const
        {
            const auto arg1_val = env.get_value(m_actual_args[0]);
            if (!arg1_val.is_valid())
            {
                return {};
            }

            const auto arg2_val = env.get_value(m_actual_args[1]);
            if (!arg2_val.is_valid())
            {
                return {};
            }

            const auto maybe_arg1 = convert_to<Arg1>()(arg1_val.get());
            if (!maybe_arg1.is_valid())
            {
                return {};
            }
            
            const auto maybe_arg2 = convert_to<Arg2>()(arg2_val.get());
            if (!maybe_arg2.is_valid())
            {
                return {};
            }

            const Arg1 arg1 = maybe_arg1.get();
            const Arg2 arg2 = maybe_arg2.get();

            const Ret ret = m_func(arg1, arg2);

            return convert_from(ret);
        }

        maybe<value_type> get_type(const environment&) const
        {
            return type_of<Ret>::type;
        }
    };

    std::pair<std::string, func_def> bind_bif(
            const char* symbol,
            std::vector<std::string> form_args,
            expression* expr)
    {
        return std::make_pair(
                std::string(symbol),
                func_def {
                    form_args,
                    std::unique_ptr<expression>(expr)
                });
    }

}

namespace moon
{
namespace interpret
{

    inline double op_plus(double lhs, double rhs) { return lhs + rhs; }
    inline double op_minus(double lhs, double rhs) { return lhs - rhs; }
    inline double op_mul(double lhs, double rhs) { return lhs * rhs; }
    inline double op_div(double lhs, double rhs) { return lhs / rhs; }

    inline environment env_create_bif()
    {
        std::map<std::string, func_def> func_defs;

        std::array<std::string, 2> arg_arr {{ "lhs" , "rhs" }};
        std::vector<std::string> arg_vec { "lhs", "rhs" };
        typedef bif_expression_2<double, double, double> bif_expr_2;

        func_defs.insert(bind_bif("+.", arg_vec, new bif_expr_2(arg_arr, op_plus)));
        func_defs.insert(bind_bif("-.", arg_vec, new bif_expr_2(arg_arr, op_minus)));
        func_defs.insert(bind_bif("*.", arg_vec, new bif_expr_2(arg_arr, op_mul)));
        func_defs.insert(bind_bif("/.", arg_vec, new bif_expr_2(arg_arr, op_div)));

        return environment(nullptr, {}, std::move(func_defs));
    }

}
}
