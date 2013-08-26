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

#include "env_bif.cpp"
#include "common/log.h"
#include "common/expr.h"

#include <unittest++/UnitTest++.h>

namespace
{
    std::unique_ptr<script::expression> make_bif_call(
            const std::string& symbol,
            std::unique_ptr<script::expression> lhs,
            std::unique_ptr<script::expression> rhs)
    {
        std::vector<std::unique_ptr<script::expression>> args;
        args.push_back(std::move(lhs));
        args.push_back(std::move(rhs));
        return script::expr_create_func_call(symbol, std::move(args));
    }
}

SUITE(BuiltinFunctionsEnvironmentTestSuite)
{
    TEST(SimpleArithmetics)
    {
        script::info("Testing env_bif::simple_arithmetics");

        auto env_base = script::env_create_bif();

        double lhs = 1.2;
        double rhs = 3.4;

        const script::value lhs_val { script::value_type::real, 0, lhs, {} };
        const script::value rhs_val { script::value_type::real, 0, rhs, {} };

        script::environment env(&env_base, { { "lhs", lhs_val }, { "rhs", rhs_val } }, {});

        auto call_add = make_bif_call("+.", expr_create_literal(lhs_val), expr_create_literal(rhs_val));
        auto add_result = call_add->eval(env);
        CHECK(add_result.is_valid());
        CHECK(add_result.get().type == script::value_type::real);
        CHECK(add_result.get().real == (lhs + rhs));

        auto call_sub = make_bif_call("-.", expr_create_literal(lhs_val), expr_create_literal(rhs_val));
        auto sub_result = call_sub->eval(env);
        CHECK(sub_result.is_valid());
        CHECK(sub_result.get().type == script::value_type::real);
        CHECK(sub_result.get().real == (lhs - rhs));

        auto call_mul = make_bif_call("*.", expr_create_literal(lhs_val), expr_create_literal(rhs_val));
        auto mul_result = call_mul->eval(env);
        CHECK(mul_result.is_valid());
        CHECK(mul_result.get().type == script::value_type::real);
        CHECK(mul_result.get().real == (lhs * rhs));

        auto call_div = make_bif_call("/.", expr_create_literal(lhs_val), expr_create_literal(rhs_val));
        auto div_result = call_div->eval(env);
        CHECK(div_result.is_valid());
        CHECK(div_result.get().type == script::value_type::real);
        CHECK(div_result.get().real == (lhs / rhs));
    }
}
