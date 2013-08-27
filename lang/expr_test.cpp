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

#include "common/log.h"
#include "interpret/env.h"
#include "expr.h"

#include <unittest++/UnitTest++.h>

namespace
{
    using namespace moon::lang;

    const double ARBITRARY_REAL_1 = 1.0;
    const double ARBITRARY_REAL_2 = 2.0;

    const std::string ARBITRARY_SYMBOL_1 = "arbitrary-symbol-1";
    const std::string ARBITRARY_SYMBOL_2 = "arbitrary-symbol-2";
    const std::string ARBITRARY_SYMBOL_3 = "arbitrary-symbol-3";

    const value ARBITRARY_VALUE_1 { value_type::real, 0, ARBITRARY_REAL_1, {} };
    const value ARBITRARY_VALUE_2 { value_type::real, 0, ARBITRARY_REAL_2, {} };
}

SUITE(ExpressionsTestSuite)
{

    using namespace moon::interpret;
    using namespace moon::lang;
    using namespace moon::log;
    using namespace moon::common;

    TEST(LiteralEvaluation)
    {
        info("Testing expressions::literal_evaluation.");

        const environment EMPTY_ENVIRONMENT(nullptr, {}, {});

        auto lit = expr_create_literal(ARBITRARY_VALUE_1);
        auto maybe_val = lit->eval(EMPTY_ENVIRONMENT);
        CHECK(maybe_val.is_valid());
        CHECK(maybe_val.get() == ARBITRARY_VALUE_1);
    }

    TEST(ReferenceEvaluation)
    {
        info("Testing expressions::reference_evaluation.");
       
        const environment SINGLE_SYMBOL_ENVIRONMENT(
            nullptr,
            { { ARBITRARY_SYMBOL_1, ARBITRARY_VALUE_1 } },
            {} );

        auto ref = expr_create_reference(ARBITRARY_SYMBOL_1);
        auto maybe_val = ref->eval(SINGLE_SYMBOL_ENVIRONMENT);
        CHECK(maybe_val.is_valid());
        CHECK(maybe_val.get() == ARBITRARY_VALUE_1);
    }

    TEST(FuncCallEvaluation)
    {
        // ACHTUNG: huge test case model!
        info("Testing expressions::func_call_evaluation.");

        struct custom_expression : public expression
        {
            maybe<value> eval(const environment& env) const
            {
                value lhs = env.get_value("lhs").get();
                value rhs = env.get_value("rhs").get();
                return {{
                    value_type::real,
                    0,
                    lhs.real + rhs.real,
                    {} }};
            }
        };

        std::unique_ptr<expression> FUNC_EXPR(new custom_expression);
        trace("\tCreated custom expression.");

        func_def FUNC_DEF { { "lhs", "rhs" }, std::move(FUNC_EXPR) };
        trace("\tCreated function definition.");

        std::map<std::string, func_def> FUNC_BINDINGS;
        FUNC_BINDINGS.insert(std::make_pair(ARBITRARY_SYMBOL_3, std::move(FUNC_DEF)));
        trace("\tCreated function binding.");

        const environment TWO_SYMBOLS_AND_FUNC_ENVIRONMENT(
            nullptr,
            {
                { ARBITRARY_SYMBOL_1, ARBITRARY_VALUE_1 },
                { ARBITRARY_SYMBOL_2, ARBITRARY_VALUE_2 }
            },
            std::move(FUNC_BINDINGS)
        );
        trace("\tCreated dummy environment.");

        std::vector<std::unique_ptr<expression>> actual_args;
        actual_args.push_back(expr_create_reference(ARBITRARY_SYMBOL_1));
        actual_args.push_back(expr_create_reference(ARBITRARY_SYMBOL_2));
        trace("\tCreated dummy args.");

        auto func_call = expr_create_func_call(
                ARBITRARY_SYMBOL_3,
                std::move(actual_args));
        trace("\tCreated the function call under test.");

        const value expected_value
        {
            value_type::real, 0, ARBITRARY_REAL_1 + ARBITRARY_REAL_2, {}
        };
        trace("\tCreated expected value.");

        auto maybe_actual_value = func_call->eval(TWO_SYMBOLS_AND_FUNC_ENVIRONMENT);

        CHECK(maybe_actual_value.is_valid());
        CHECK(maybe_actual_value.get() == expected_value );
        trace("\tCalled the function in the dummy environment.");

    }

}

