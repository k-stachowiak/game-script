#ifndef MOON_PARSE_SEXPR_TOKEN_H
#define MOON_PARSE_SEXPR_TOKEN_H

#include <string>

namespace moon {
namespace parse {

	enum ESexprTokenCharacter {
		TOK_CORE_OPEN = '(',
		TOK_CORE_CLOSE = ')',
		TOK_LIST_OPEN = '{',
		TOK_LIST_CLOSE = '}',
		TOK_ARR_OPEN = '[',
		TOK_ARR_CLOSE = ']',
		TOK_TUP_OPEN = '<',
		TOK_TUP_CLOSE = '>',
		TOK_COMMENT = '#',
		TOK_STR_DELIM = '"',
		TOK_CHAR_DELIM = '\'',
		TOK_DELIM_ESCAPE = '\\'
	};

	class CSexprToken
	{
		const char* m_begin;
		const char* m_end;

	public:
		CSexprToken(const char* begin, const char* end) :
			m_begin(begin),
			m_end(end)
		{}

		std::string ToString() const
		{
			return std::string{ m_begin, m_end };
		}

		bool IsCharacter(char c) const
		{
			return (m_end - m_begin) == 1 && *m_begin == c;
		}

		bool IsOpeningParenthesis() const
		{
			return IsCharacter(TOK_CORE_OPEN) ||
				IsCharacter(TOK_LIST_OPEN) ||
				IsCharacter(TOK_ARR_OPEN) ||
				IsCharacter(TOK_TUP_OPEN);
		}

		bool IsClosingParenthesis() const
		{
			return IsCharacter(TOK_CORE_CLOSE) ||
				   IsCharacter(TOK_LIST_CLOSE) ||
				   IsCharacter(TOK_ARR_CLOSE) ||
				   IsCharacter(TOK_TUP_CLOSE);
		}
	};

	inline bool g_ParenthesisMatch(const CSexprToken& lhs, const CSexprToken& rhs)
	{
		return (lhs.IsCharacter(TOK_CORE_OPEN) && rhs.IsCharacter(TOK_CORE_CLOSE)) ||
			(lhs.IsCharacter(TOK_LIST_OPEN) && rhs.IsCharacter(TOK_LIST_CLOSE)) ||
			(lhs.IsCharacter(TOK_ARR_OPEN) && rhs.IsCharacter(TOK_ARR_CLOSE)) ||
			(lhs.IsCharacter(TOK_TUP_OPEN) && rhs.IsCharacter(TOK_TUP_CLOSE));
	}

	inline bool g_DelimiterMatch(const CSexprToken& lhs, const CSexprToken& rhs)
	{
		return (lhs.IsCharacter(TOK_STR_DELIM) && rhs.IsCharacter(TOK_STR_DELIM)) ||
			(lhs.IsCharacter(TOK_CHAR_DELIM) && rhs.IsCharacter(TOK_CHAR_DELIM));
	}
}
}

#endif