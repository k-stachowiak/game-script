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

#include "env_build.h"
#include "env_build.cpp"

#include <unittest++/UnitTest++.h>

namespace
{
    // Mutually different strings without white spaces.
    const std::string ARBITRARY_SYMBOL_1 = "arbitrary-symbol-1";
    const std::string ARBITRARY_SYMBOL_2 = "arbitrary-symbol-2";
    const std::string ARBITRARY_SYMBOL_3 = "arbitrary-symbol-3";
    const std::string ARBITRARY_SYMBOL = ARBITRARY_SYMBOL_1;

    // Valid primitive literals.
    const std::string INTEGER_LITERAL = "1";
    const std::string REAL_LITERAL = "2.0e0";
    const std::string STRING_LITERAL = "\"3\"";
    const std::string ANY_LITERAL = INTEGER_LITERAL;

    // Misc.
    const std::string ARBITRARY_STRING = "arbitrary string";

    // Equivalent : ()
    const script::node EMPTY_LIST { script::node_type::list, {}, {} };

    const script::node ANY_LIST = EMPTY_LIST;

    // Equivalent : arbitrary-symbol-1
    const script::node ANY_ATOM { script::node_type::atom, ARBITRARY_SYMBOL_1, {} };

    // Helper constructors.

    script::node make_atom_node(const std::string& str)
    {
        return script::node { script::node_type::atom, { str }, {} };
    }

    script::node make_list_node(const std::vector<script::node>& nodes)
    {
        return script::node { script::node_type::list, {}, nodes };
    }
}

SUITE(AstBuilderTestSuite)
{

    // Specific expression parsing tests.
    // ----------------------------------

    TEST(LiteralParsing)
    {
        std::unique_ptr<script::expression> expr;

        // 1. Fail on list node.
        expr = try_parse_literal(ANY_LIST);
        CHECK(static_cast<bool>(expr) == false);

        // 2. Fail on invalid literal.
        expr = try_parse_literal(make_atom_node(ARBITRARY_SYMBOL));
        CHECK(static_cast<bool>(expr) == false);

        expr = try_parse_literal(make_atom_node(REAL_LITERAL + ARBITRARY_STRING));
        CHECK(static_cast<bool>(expr) == false);

        // 3. String literal.
        expr = try_parse_literal(make_atom_node(STRING_LITERAL));
        CHECK(static_cast<bool>(expr) == true);

        // 4. Integer literal.
        expr = try_parse_literal(make_atom_node(INTEGER_LITERAL));
        CHECK(static_cast<bool>(expr) == true);

        // 5. Real literal.
        expr = try_parse_literal(make_atom_node(REAL_LITERAL));
        CHECK(static_cast<bool>(expr) == true);

    }

    TEST(ReferenceParsing)
    {
        std::unique_ptr<script::expression> expr;

        // 1. Fail on list node. 
        expr = try_parse_reference(ANY_LIST);
        CHECK(static_cast<bool>(expr) == false);

        // 2. Fail on atom with digit in front.
        expr = try_parse_reference(make_atom_node(INTEGER_LITERAL));
        CHECK(static_cast<bool>(expr) == false);

        // 3. Valid reference symbol.
        expr = try_parse_reference(make_atom_node(ARBITRARY_SYMBOL_1));
        CHECK(static_cast<bool>(expr) == true);
    }

    TEST(FunctionCallParsing)
    {
        std::unique_ptr<script::expression> expr;

        // 1. Fail on atom.
        expr = try_parse_func_call(ANY_ATOM);
        CHECK(static_cast<bool>(expr) == false);

        // 2. Fail on empty list.
        expr = try_parse_func_call(EMPTY_LIST);
        CHECK(static_cast<bool>(expr) == false);

        // 3. Valid function call - no arguments.
        auto simple_list = make_list_node({ make_atom_node(ARBITRARY_SYMBOL_1) });
        expr = try_parse_func_call(simple_list);
        CHECK(static_cast<bool>(expr) == true);

        // 4. Valid function call - with arguments.
        auto complex_list = make_list_node(
        {
            make_atom_node(ARBITRARY_SYMBOL_1), // Pretend to be the function name
            make_atom_node(ARBITRARY_SYMBOL_2), // Pretend to be a reference argument
            make_atom_node(ANY_LITERAL)         // Pretend to be a literal argument
        });
        expr = try_parse_func_call(complex_list);
        CHECK(static_cast<bool>(expr) == true);
    }

}

