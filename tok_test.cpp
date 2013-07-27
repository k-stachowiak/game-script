#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <unittest++/UnitTest++.h>

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

    // All white spaces checked by the std::iswspace(...).
    const std::string ALL_WSPACES = " \f\n\r\t\v";

    // Anything but new line.
    const std::string COMMENT_STRING = "; some irrelevant text";

}

SUITE(TokenizerTestSuite)
{
    TEST(SingleTokens)
    {
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
    }

    TEST(SimpleListNoSpaces)
    {
        auto actual_tokens = script::tokenize("(" + ARBITRARY_TOKEN + ")");
        std::vector<std::string> expected_tokens { "(", ARBITRARY_TOKEN, ")" };
        CHECK(vec_equal(actual_tokens, expected_tokens));
    }

    TEST(SimpleListAllSpaces)
    {
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

int main()
{
    return UnitTest::RunAllTests();
}

