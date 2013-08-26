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

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <unittest++/UnitTest++.h>

#include "common/log.h"
#include "common/except.h"
#include "tokenize.h"
#include "tok.h"

namespace {

    template<class T1, class T2>
    bool vec_equal(const std::vector<T1>& lhs, const std::vector<T2>& rhs)
    {
        return lhs.size() == rhs.size() &&
               std::equal(begin(lhs), end(lhs), begin(rhs));
    }

    template<class T>
    void vec_print(const std::vector<T>& v, std::ostream& out)
    {
        for(const auto& x : v) out << x << " ";
    }

    // Anything but white spaces.
    const std::string ARBITRARY_TOKEN = "ArbitraryToken";

    // String with white space.
    const std::string WSPACE_STRING = "string with white spaces";

    // The delimiter string.
    const std::string DELIM(1, static_cast<char>(script::token_char::strdelim));

    // The comment char string.
    const std::string COMMENT_DELIM(1, static_cast<char>(script::token_char::comment));

    // String with escaped doublequote.
    const std::string ESC_DQUOTE = "string with escaped '\\" + DELIM + "'";

    // Delimited atom.
    const std::string DELIMITED_ATOM = DELIM + WSPACE_STRING + DELIM;

    // Delimited atom with escaped delimiter.
    const std::string DELIMITED_ATOM_WESC = DELIM + ESC_DQUOTE + DELIM;

    // Empty delimited atom.
    const std::string EMPTY_DELIMITED_ATOM = DELIM + DELIM;

    // Unclosed delim atom.
    const std::string UNCLOSED_DELIM_ATOM = DELIM + ARBITRARY_TOKEN;

    // All white spaces checked by the std::iswspace(...).
    const std::string ALL_WSPACES = " \f\n\r\t\v";

    // Anything but new line.
    const std::string COMMENT_STRING = COMMENT_DELIM + " some irrelevant text";

}

SUITE(TokenizerTestSuite)
{
    TEST(SingleTokens)
    {
        script::info("Testing tokenizer::simple_tokens");

        {
            std::vector<std::string> actual_tokens = script::tokenize("(");
            std::vector<std::string> expected_tokens { "(" };
            CHECK(vec_equal(actual_tokens, expected_tokens));
        }

        {
            std::vector<std::string> actual_tokens = script::tokenize(")");
            std::vector<std::string> expected_tokens { ")" };
            CHECK(vec_equal(actual_tokens, expected_tokens));
        }

        {
            std::vector<std::string> actual_tokens = script::tokenize(ARBITRARY_TOKEN);
            std::vector<std::string> expected_tokens { ARBITRARY_TOKEN };
            CHECK(vec_equal(actual_tokens, expected_tokens));
        }

        {
            std::vector<std::string> actual_tokens = script::tokenize(EMPTY_DELIMITED_ATOM);
            std::vector<std::string> expected_tokens { std::string {} };
            CHECK(vec_equal(actual_tokens, expected_tokens));
        }

        {
            std::vector<std::string> actual_tokens = script::tokenize(DELIMITED_ATOM);
            std::vector<std::string> expected_tokens { WSPACE_STRING };
            CHECK(vec_equal(actual_tokens, expected_tokens));
        }

        {
            std::vector<std::string> actual_tokens = script::tokenize(DELIMITED_ATOM_WESC);
            std::vector<std::string> expected_tokens { ESC_DQUOTE };
            CHECK(vec_equal(actual_tokens, expected_tokens));
        }

        CHECK_THROW
        (
            script::tokenize(UNCLOSED_DELIM_ATOM),
            script::unclosed_delimited_token
        );
    }

    TEST(SimpleListNoSpaces)
    {
        script::info("Testing tokenizer::simple_list_no_spaces");
        auto actual_tokens = script::tokenize("(" + ARBITRARY_TOKEN + ")");
        std::vector<std::string> expected_tokens { "(", ARBITRARY_TOKEN, ")" };
        CHECK(vec_equal(actual_tokens, expected_tokens));
    }

    TEST(SimpleListAllSpaces)
    {
        script::info("Testing tokenizer::simple_list_with_spaces");
        auto actual_tokens = script::tokenize(
                    ALL_WSPACES + "(" +
                    ALL_WSPACES + ARBITRARY_TOKEN + 
                    ALL_WSPACES + ")" +
                    ALL_WSPACES);

        std::vector<std::string> expected_tokens { "(", ARBITRARY_TOKEN, ")" };

        CHECK(vec_equal(actual_tokens, expected_tokens));
    }

    TEST(CommentInterruption)
    {
        script::info("Testing tokenizer::comment_interruption");
        {
            auto actual_tokens = script::tokenize(
                    "(" + COMMENT_STRING + "\n)");

            std::vector<std::string> expected_tokens { "(", ")" };

            CHECK(vec_equal(actual_tokens, expected_tokens));
        }

        {
            auto actual_tokens = script::tokenize(
                    ARBITRARY_TOKEN + COMMENT_STRING + "\n(");

            std::vector<std::string> expected_tokens { ARBITRARY_TOKEN, "(" };

            CHECK(vec_equal(actual_tokens, expected_tokens));
        }

        {
            auto actual_tokens = script::tokenize( 
                    ARBITRARY_TOKEN +
                    ALL_WSPACES + COMMENT_STRING +
                    ALL_WSPACES + ARBITRARY_TOKEN);

            std::vector<std::string> expected_tokens
            {
                ARBITRARY_TOKEN,
                ARBITRARY_TOKEN
            };

            CHECK(vec_equal(actual_tokens, expected_tokens));
        }
    }
}

