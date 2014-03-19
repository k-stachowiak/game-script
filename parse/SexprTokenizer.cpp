#include <cctype>

#include <iterator>
#include <algorithm>

#include "SexprTokenizer.h"

namespace moon {
namespace parse {

	static inline bool IsAnyParenthesis(int c)
	{
		return c == TOK_CORE_OPEN ||
			   c == TOK_CORE_CLOSE ||
			   c == TOK_LIST_OPEN ||
			   c == TOK_LIST_CLOSE ||
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
	Iter FindNonescapedDelimiter(Iter current, Iter last, char delimiter)
	{
		while (current != last && (
			*current != TOK_STR_DELIM || (
				*current == TOK_STR_DELIM &&
			    *(current - 1) == TOK_DELIM_ESCAPE))) {
			++current;
		}
		return current;
	}

	template <class Out>
	CStrIter TryParseParenthesis(CStrIter current, CStrIter last, Out out)
	{
		if (current == last || !IsAnyParenthesis(*current)) {
			return current;
		}

		*(out++) = CSexprToken{ current, current + 1 };

		return std::find_if_not(current, last, isspace);
	}

	template <class Out>
	CStrIter TryParseRegularAtom(CStrIter current, CStrIter last, Out out)
	{
		if (current == last || !IsAllowedInAtom(*current)) {
			return current;
		}
		
		auto atomEnd = std::find_if_not(current, last, IsAllowedInAtom);

		*(out++) = CSexprToken{ current, atomEnd };

		return std::find_if_not(current, last, isspace);
	}

	template <class Out>
	CStrIter TryParseDelimitedAtom(CStrIter current, CStrIter last, Out out)
	{
		if (current == last || (*current != TOK_STR_DELIM &&
							    *current != TOK_CHAR_DELIM)) {
			return current;
		}

		char delimiter = *current;
		auto atomEnd = FindNonescapedDelimiter(current, last, delimiter);

		if (current == last) {
			throw std::runtime_error{ "Non-delimited string/character atom" };
		}

		*(out++) = CSexprToken{ current, atomEnd };

		return std::find_if_not(current, last, isspace);
	}

	CStrIter TryParseComments(CStrIter current, CStrIter last)
	{
		if (current == last || *current != TOK_COMMENT) {
			return current;
		}

		current = std::find(current, last, '\n');

		return std::find_if_not(current, last, isspace);
	}


	std::vector<CSexprToken> CSexprTokenizer::Tokenize(CStrIter current, const CStrIter& last)
	{
		std::vector<CSexprToken> result;
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