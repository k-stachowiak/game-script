#include <cctype>

#include <iterator>
#include <algorithm>

#include "Exceptions.h"
#include "Algorithm.h"

namespace moon {
namespace parse {
namespace sexpr {

    static inline bool IsAnyParenthesis(int c)
    {
        return c == TOK_CORE_OPEN ||
               c == TOK_CORE_CLOSE ||
               c == TOK_ARR_OPEN ||
               c == TOK_ARR_CLOSE ||
               c == TOK_TUP_OPEN ||
               c == TOK_TUP_CLOSE;
    }

    static inline bool IsAllowedInAtom(int c)
    {
        return !IsAnyParenthesis(c) &&
               !isspace(c) &&
               c != TOK_STR_DELIM &&
               c != TOK_DELIM_ESCAPE &&
               c != TOK_COMMENT;
    }

    template <class Iter>
    static Iter FindNonescapedDelimiter(Iter current, Iter last, char delim)
    {
        while (current != last && (*current != delim || *(current - 1) == TOK_DELIM_ESCAPE)) {
            ++current;
        }
        return current;
    }

    template <class Out>
    static StrIter TryParseParenthesis(StrIter current, StrIter last, Out out)
    {
        if (current == last || !IsAnyParenthesis(*current)) {
            return current;
        }

        *(out++) = Token{ 
            SourceLocation::MakeRegular(
                current.GetLine(),
                current.GetColumn()),
            current, current + 1 };

        return std::find_if_not(current + 1, last, isspace);
    }

    template <class Out>
    static StrIter TryParseRegularAtom(StrIter current, StrIter last, Out out)
    {
        if (current == last || !IsAllowedInAtom(*current)) {
            return current;
        }
        
        auto atomEnd = std::find_if_not(current, last, IsAllowedInAtom);

        *(out++) = Token{
            SourceLocation::MakeRegular(
                current.GetLine(),
                current.GetColumn()),
            current, atomEnd };

        return std::find_if_not(atomEnd, last, isspace);
    }

    template <class Out>
    static StrIter TryParseDelimitedAtom(StrIter current, StrIter last, Out out)
    {
        if (current == last || (*current != TOK_STR_DELIM &&
                                *current != TOK_CHAR_DELIM)) {
            return current;
        }

        char delimiter = *current;
        auto atomEnd = FindNonescapedDelimiter(current + 1, last, delimiter);

        if (atomEnd == last || (*atomEnd != delimiter)) {
            throw ExTokNonDelimitedStringOrCharacter{
                SourceLocation::MakeRegular(
                    current.GetLine(),
                    current.GetColumn())
            };
        }

        *(out++) = Token{
            SourceLocation::MakeRegular(
                current.GetLine(),
                current.GetColumn()),
            current, atomEnd + 1 };

        return std::find_if_not(atomEnd + 1, last, isspace);
    }

    static StrIter TryParseComments(StrIter current, StrIter last)
    {
        if (current == last || *current != TOK_COMMENT) {
            return current;
        }

        current = std::find(current, last, '\n');

        return std::find_if_not(current, last, isspace);
    }


    std::vector<Token> Tokenize(StrIter current, const StrIter& last)
    {
        std::vector<Token> result;
        auto inserter = std::back_inserter(result);

        current = std::find_if_not(current, last, isspace);

        while (current != last) {
            current = TryParseParenthesis(current, last, inserter);
            current = TryParseRegularAtom(current, last, inserter);
            current = TryParseDelimitedAtom(current, last, inserter);
            current = TryParseComments(current, last);
        }

        return result;
    }

}
}
}
