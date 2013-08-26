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
#include "log.h"
#include "interpret/env.h"

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
        script::info("Testing expressions::literal_evaluation.");

        const script::environment EMPTY_ENVIRONMENT(nullptr, {}, {});

        auto lit = script::expr_create_literal(ARBITRARY_VALUE_1);
        auto maybe_val = lit->eval(EMPTY_ENVIRONMENT);
        CHECK(maybe_val.is_valid());
        CHECK(maybe_val.get() == ARBITRARY_VALUE_1);
    }

    TEST(ReferenceEvaluation)
    {
        script::info("Testing expressions::reference_evaluation.");
       
        const script::environment SINGLE_SYMBOL_ENVIRONMENT(
            nullptr,
            { { ARBITRARY_SYMBOL_1, ARBITRARY_VALUE_1 } },
            {} );

        auto ref = script::expr_create_reference(ARBITRARY_SYMBOL_1);
        auto maybe_val = ref->eval(SINGLE_SYMBOL_ENVIRONMENT);
        CHECK(maybe_val.is_valid());
        CHECK(maybe_val.get() == ARBITRARY_VALUE_1);
    }

    TEST(FuncCallEvaluation)
    {
        // ACHTUNG: huge test case model!
        script::info("Testing expressions::func_call_evaluation.");

        struct custom_expression : public script::expression
        {
            script::maybe<script::value> eval(const script::environment& env) const
            {
                script::value lhs = env.get_value("lhs").get();
                script::value rhs = env.get_value("rhs").get();
                return {{
                    script::value_type::real,
                    0,
                    lhs.real + rhs.real,
                    {} }};
            }

            script::maybe<script::value_type> get_type(const script::environment&) const
            {
                return {{}};
            }
        };

        std::unique_ptr<script::expression> FUNC_EXPR(new custom_expression);
        script::trace("\tCreated custom expression.");

        script::func_def FUNC_DEF { { "lhs", "rhs" }, std::move(FUNC_EXPR) };
        script::trace("\tCreated function definition.");

        std::map<std::string, script::func_def> FUNC_BINDINGS;
        FUNC_BINDINGS.insert(std::make_pair(ARBITRARY_SYMBOL_3, std::move(FUNC_DEF)));
        script::trace("\tCreated function binding.");

        const script::environment TWO_SYMBOLS_AND_FUNC_ENVIRONMENT(
            nullptr,
            {
                { ARBITRARY_SYMBOL_1, ARBITRARY_VALUE_1 },
                { ARBITRARY_SYMBOL_2, ARBITRARY_VALUE_2 }
            },
            std::move(FUNC_BINDINGS)
        );
        script::trace("\tCreated dummy environment.");

        std::vector<std::unique_ptr<script::expression>> actual_args;
        actual_args.push_back(script::expr_create_reference(ARBITRARY_SYMBOL_1));
        actual_args.push_back(script::expr_create_reference(ARBITRARY_SYMBOL_2));
        script::trace("\tCreated dummy args.");

        auto func_call = script::expr_create_func_call(
                ARBITRARY_SYMBOL_3,
                std::move(actual_args));
        script::trace("\tCreated the function call under test.");

        const script::value expected_value
        {
            script::value_type::real, 0, ARBITRARY_REAL_1 + ARBITRARY_REAL_2, {}
        };
        script::trace("\tCreated expected value.");

        auto maybe_actual_value = func_call->eval(TWO_SYMBOLS_AND_FUNC_ENVIRONMENT);

        CHECK(maybe_actual_value.is_valid());
        CHECK(maybe_actual_value.get() == expected_value );
        script::trace("\tCalled the function in the dummy environment.");

    }

}

