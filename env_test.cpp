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

#include <unittest++/UnitTest++.h>

#include "env.h"

namespace
{
    const std::string NON_EXISTING_KEY = "non-existing key";

    const std::string EXISTING_KEY_1 = "existing-key-1";
    const std::string EXISTING_KEY_2 = "existing-key-2";

    const double ARBITRARY_REAL_1 = 1.0;
    const double ARBITRARY_REAL_2 = 2.0;

    const script::value ARBITRARY_VALUE_1 { script::value_type::real, 0, ARBITRARY_REAL_1, {} };
    const script::value ARBITRARY_VALUE_2 { script::value_type::real, 0, ARBITRARY_REAL_2, {} };
}

SUITE(EnvironmentTestSuite)
{

    TEST(ValuesInSimpleStructure)
    {
        script::environment env(nullptr, { { EXISTING_KEY_1, ARBITRARY_VALUE_1 } }, {});

        CHECK(env.has_value(NON_EXISTING_KEY) == false);
        CHECK(env.has_value(EXISTING_KEY_1) == true);
        CHECK(env.get_value(EXISTING_KEY_1) == ARBITRARY_VALUE_1);
    }

    TEST(ValuesInComplexStructure)
    {
        script::environment base(nullptr, { { EXISTING_KEY_1, ARBITRARY_VALUE_1 } }, {});
        script::environment derived(&base, { { EXISTING_KEY_2, ARBITRARY_VALUE_2 } }, {});

        CHECK(derived.has_value(NON_EXISTING_KEY) == false);
        CHECK(derived.has_value(EXISTING_KEY_1) == true);
        CHECK(derived.get_value(EXISTING_KEY_1) == ARBITRARY_VALUE_1);
        CHECK(derived.has_value(EXISTING_KEY_2) == true);
        CHECK(derived.get_value(EXISTING_KEY_2) == ARBITRARY_VALUE_2);
    }

}

