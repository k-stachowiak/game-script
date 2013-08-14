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
#include <iterator>

#include "tokenize.h"
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
        return std::string { tok.first, tok.last };
    }

    // The generic tokenizer implementation.
    template<class InIter, class OutIter>
    void tokenize(InIter first, InIter last, OutIter out)
    {

        // NOTE that the only way used to proceed the current iterator here
        // is by the STL algorithms. This minimizes the risk of buffer
        // overrun as we always proceed against an explicit end iterator.

        const char lopen = static_cast<char>(script::token_char::lopen);
        const char lclose = static_cast<char>(script::token_char::lclose);
        const char comment = static_cast<char>(script::token_char::comment);
        const char strdelim = static_cast<char>(script::token_char::strdelim);
        const char strescape = static_cast<char>(script::token_char::strescape);

        auto current = first;

        // 1. Skip the initial white spaces.
        current = std::find_if_not(current, last, std::iswspace);
        if (current == last) return;

        // 2. Tokenizing loop.
        while (current != last)
        {
            // 2.1. Parenthesis cases.
            if (*current == lopen || *current == lclose)
            {
                *out++ = token<InIter> { current, current + 1 };
                current = std::find_if_not(current + 1, last, std::iswspace);
                if (current == last) return;
            }

            // 2.2. Comment case.
            else if (*current == comment)
            {
                auto end_line = std::find(current, last, '\n');
                // *out++ = { current, end_line };
                current = std::find_if_not(end_line + 1, last, std::iswspace);
                if (current == last) return;
            }

            // 2.3. Delimited atom case.
            else if (*current == strdelim)
            {
                auto end_atom = current;

                do 
                {
                    end_atom = std::find(end_atom + 1, last, strdelim);
                    if(end_atom == last) return; // FIXME: This is actually an error!

                } while (*(end_atom - 1) == strescape);

                *out++ = { current + 1, end_atom };

                current = std::find_if_not(end_atom + 1, last, std::iswspace);

                if(current == last) return; // FIXME: This is actually an error!
            }

            // 2.4. Regular atom case.
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
