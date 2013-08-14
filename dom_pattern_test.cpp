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

#include "dom_pattern.h"

#include <unittest++/UnitTest++.h>

namespace
{

    const std::string ARBITRARY_ATOM1 = "arbitrary-atom1";
    const std::string ARBITRARY_ATOM2 = "arbitrary-atom2";

}

SUITE(DomExpectedTestSuite)
{

    TEST(SimpleMatch)
    {
        script::node string_node1 { script::node_type::atom, ARBITRARY_ATOM1, {} };
        script::node string_node2 { script::node_type::atom, ARBITRARY_ATOM2, {} };

        script::pattern_spec_atom exp_str { ARBITRARY_ATOM1 };

        CHECK(match(exp_str, string_node1));
        CHECK(!match(exp_str, string_node2));
    }

    TEST(CaptureMatch)
    {
        script::node string_node { script::node_type::atom, ARBITRARY_ATOM1, {} };

        script::node list_node {
            script::node_type::list,
            {},
            {
                {
                    script::node_type::atom,
                    ARBITRARY_ATOM1,
                    {}
                },
                {
                    script::node_type::atom,
                    ARBITRARY_ATOM2,
                    {}
                }
            }
        };

        std::string captured;
        script::pattern_atom_cap exp_cap { captured };

        CHECK(!match(exp_cap, list_node));
        CHECK(match(exp_cap, string_node));
        CHECK(captured == ARBITRARY_ATOM1);
    }

    TEST(ComplexMatch)
    {

        script::node list_node {
            script::node_type::list,
            {},
            {
                {
                    script::node_type::atom,
                    ARBITRARY_ATOM1,
                    {}
                },
                {
                    script::node_type::atom,
                    ARBITRARY_ATOM2,
                    {}
                }
            }
        };

        script::pattern_spec_atom exp_ast { ARBITRARY_ATOM1 };
        script::pattern_atom exp_any;

        script::pattern_list_of<script::pattern_spec_atom, script::pattern_atom> exp_list1
        {
           std::make_tuple(exp_ast, exp_any)
        };
        
        script::pattern_list_of<script::pattern_atom, script::pattern_atom> exp_list2
        {
            std::make_tuple(exp_any, exp_any)
        };

        CHECK(match(exp_list1, list_node));
        CHECK(match(exp_list2, list_node));
    }

}

int main()
{
    return UnitTest::RunAllTests();
}
