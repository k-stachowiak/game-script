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

#include "expr.h"
#include "env.h"

#include <unittest++/UnitTest++.h>

namespace
{
    const double ARBITRARY_REAL_1 = 1.0;
    const double ARBITRARY_REAL_2 = 2.0;

    const std::string ARBITRARY_SYMBOL_1 = "arbitrary-symbol-1";
    const std::string ARBITRARY_SYMBOL_2 = "arbitrary-symbol-2";
    const std::string ARBITRARY_SYMBOL_3 = "arbitrary-symbol-3";

    const script::value ARBITRARY_VALUE_1 { script::value_type::real, 0, ARBITRARY_REAL_1, {} };
    const script::value ARBITRARY_VALUE_2 { script::value_type::real, 0, ARBITRARY_REAL_2, {} };
}

SUITE(ExpressionsTestSuite)
{

    TEST(LiteralEvaluation)
    {
        const script::environment EMPTY_ENVIRONMENT(nullptr, {}, {});

        auto lit = script::expr_create_literal(ARBITRARY_VALUE_1);
        CHECK(lit->eval(EMPTY_ENVIRONMENT) == ARBITRARY_VALUE_1);
    }

    TEST(ReferenceEvaluation)
    {
        const script::environment SINGLE_SYMBOL_ENVIRONMENT(
            nullptr,
            { { ARBITRARY_SYMBOL_1, ARBITRARY_VALUE_1 } },
            {} );

        auto ref = script::expr_create_reference(ARBITRARY_SYMBOL_1);
        CHECK(ref->eval(SINGLE_SYMBOL_ENVIRONMENT) == ARBITRARY_VALUE_1);
    }

    TEST(FuncCallEvaluation)
    {
        // ACHTUNG: huge test case model!

        struct custom_expression : public script::expression
        {
            script::value eval(const script::environment& env) const
            {
                script::value lhs = env.get_value("lhs");
                script::value rhs = env.get_value("rhs");
                return {
                    script::value_type::real,
                    0,
                    lhs.real + rhs.real,
                    {} };
            }
        };

        std::unique_ptr<script::expression> FUNC_EXPR(new custom_expression);

        script::func_def FUNC_DEF { { "lhs", "rhs" }, std::move(FUNC_EXPR) };

        std::map<std::string, script::func_def> FUNC_BINDINGS;
        FUNC_BINDINGS.insert(std::make_pair(ARBITRARY_SYMBOL_3, std::move(FUNC_DEF)));

        const script::environment TWO_SYMBOLS_AND_FUNC_ENVIRONMENT(
            nullptr,
            {
                { ARBITRARY_SYMBOL_1, ARBITRARY_VALUE_1 },
                { ARBITRARY_SYMBOL_2, ARBITRARY_VALUE_2 }
            },
            std::move(FUNC_BINDINGS)
        );

        std::vector<std::unique_ptr<script::expression>> actual_args;
        actual_args.push_back(script::expr_create_reference(ARBITRARY_SYMBOL_1));
        actual_args.push_back(script::expr_create_reference(ARBITRARY_SYMBOL_2));

        auto func_call = script::expr_create_func_call(
                ARBITRARY_SYMBOL_3,
                std::move(actual_args));

        const script::value SUM_OF_ARBITRARY_1_AND_2 =
        {
            script::value_type::real, 0, ARBITRARY_REAL_1 + ARBITRARY_REAL_2, {}
        };

        CHECK(func_call->eval(TWO_SYMBOLS_AND_FUNC_ENVIRONMENT) == SUM_OF_ARBITRARY_1_AND_2);

    }
        

}

