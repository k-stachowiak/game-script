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

#include "common/except.h"
#include "common/log.h"
#include "dom_build.h"
#include "tok.h"

#include <iostream>
#include <unittest++/UnitTest++.h>

namespace
{
    const std::string ARBITRARY_ATOM_STRING = "arbitrary_atom_string";
    const std::string& ANY_ATOM_STRING = ARBITRARY_ATOM_STRING;
}

SUITE(DomBuilderTestSuite)
{
    using namespace moon::log;
    using namespace moon::parse;
    using namespace moon::except;

    TEST(EmptyList)
    {
        info("Testing dom_builder::empty_list");
        std::vector<std::string> tokens {};
        node actual_root = build_dom_tree(tokens);
        node expected_root { node_type::list, {}, {} };
        CHECK(expected_root == actual_root);
    }

    TEST(SingleAtomList)
    {
        info("Testing dom_builder::atom_list");
        std::vector<std::string> tokens { ARBITRARY_ATOM_STRING };

        node actual_root = build_dom_tree(tokens);

        node expected_root {
            node_type::list,
            {},
            {{
                node_type::atom,
                ARBITRARY_ATOM_STRING,
                {}
            }}
        };

        CHECK(expected_root == actual_root);
    }

    TEST(UnclosedList)
    {
        info("Testing dom_builder::unclosed_list");
        std::vector<std::string> tokens { "(", ANY_ATOM_STRING };
        CHECK_THROW
        (
            build_dom_tree(tokens),
            unclosed_dom_list
        );
    }

    TEST(SubList)
    {
        info("Testing dom_builder::sub_list");
        std::vector<std::string> tokens
        {
            ARBITRARY_ATOM_STRING,
            "(",
            ARBITRARY_ATOM_STRING,
            ARBITRARY_ATOM_STRING,
            ")"
        };

        node actual_root = build_dom_tree(tokens);

        node expected_root {
            node_type::list,
            {},
            {
                {
                    node_type::atom,
                    ARBITRARY_ATOM_STRING,
                    {}
                },
                {
                    node_type::list,
                    {},
                    {
                        {
                            node_type::atom,
                            ARBITRARY_ATOM_STRING,
                            {}
                        },
                        {
                            node_type::atom,
                            ARBITRARY_ATOM_STRING,
                            {}
                        },
                    }
                }
            }
        };

        CHECK(expected_root == actual_root);
    }
}

