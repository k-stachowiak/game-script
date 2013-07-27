#include <algorithm>
#include <iterator>

#include "tok.h"

namespace
{

    // Helper structure for efficient token representation.
    template<class Iter>
    struct token {
        Iter first;
        Iter last;
    };

    // Helper for atom recognition.
    inline bool allowed_in_atom(char c)
    {
        return c != static_cast<char>(script::token_char::lopen) &&
               c != static_cast<char>(script::token_char::lclose) &&
               c != static_cast<char>(script::token_char::comment) &&
               !iswspace(c);
    }

    // Helper token to string conversion
    template<class Iter>
    std::string tok_to_str(const token<Iter>& tok)
    {
        return std::string{ tok.first, tok.last };
    }

    // The generic tokenizer implementation.
    template<class InIter, class OutIter>
    void tokenize(InIter first, InIter last, OutIter out)
    {

        // NOTE that the only way used to proceed the current iterator here
        // is by the STL algorithms. This minimizes the risk of buffer
        // overrun as we always proceed against an explicit end iterator.

        auto current = first;

        // 1. Skip the initial white spaces.
        current = std::find_if_not(current, last, std::iswspace);
        if (current == last) return;

        // 2. Tokenizing loop.
        while (current != last)
        {
            // 2.1. Parenthesis cases.
            if (*current == static_cast<char>(script::token_char::lopen) ||
                *current == static_cast<char>(script::token_char::lclose))
            {
                *out++ = token<InIter> { current, current + 1 };
                current = std::find_if_not(current + 1, last, std::iswspace);
                if (current == last) return;
            }

            // 2.2. Comment case.
            else if (*current == static_cast<char>(script::token_char::comment))
            {
                auto end_line = std::find(current, last, '\n');
                // *out++ = { current, end_line };
                current = std::find_if_not(end_line + 1, last, std::iswspace);
                if (current == last) return;
            }

            // 2.3. Atom case.
            else
            {
                auto end_atom = std::find_if_not(current, last, allowed_in_atom);
                *out++ = { current, end_atom };
                current = std::find_if_not(end_atom, last, std::iswspace);
                if (current == last) return;
            }

            // NOTE that we consume all the white spaces at the begining of the
            // document as well as after each token, therefore no special section
            // for handling of the whitespaces is needed.
        }
    }

}

namespace script
{
    // Vector loading adapter.
    // -----------------------

    std::vector<std::string> tokenize(const std::string& str)
    {
        // Tokenize.
        std::vector<token<std::string::const_iterator>> raw_result;
        tokenize(begin(str), end(str),
                 std::back_inserter(raw_result));

        // Generalize the result collection.
        std::vector<std::string> result;
        std::transform(
                begin(raw_result), end(raw_result),
                std::back_inserter(result),
                tok_to_str<std::string::const_iterator>);

        return result;
    }

}
