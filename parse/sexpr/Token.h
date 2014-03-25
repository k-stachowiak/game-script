#ifndef MOON_PARSE_SEXPR_TOKEN_H
#define MOON_PARSE_SEXPR_TOKEN_H

#include <string>

namespace moon {
namespace parse {
namespace sexpr {

	enum ETokenCharacter {
		TOK_CORE_OPEN = '(',
		TOK_CORE_CLOSE = ')',
		TOK_ARR_OPEN = '[',
		TOK_ARR_CLOSE = ']',
		TOK_TUP_OPEN = '<',
		TOK_TUP_CLOSE = '>',
		TOK_COMMENT = '#',
		TOK_STR_DELIM = '"',
		TOK_CHAR_DELIM = '\'',
		TOK_DELIM_ESCAPE = '\\'
	};

	class CToken
	{
		const int m_line;
		const int m_column;

		const char* m_begin;
		const char* m_end;

	public:
		CToken(int line, int column, const char* begin, const char* end) :
			m_line{ line },
			m_column{ column },
			m_begin{ begin },
			m_end{ end }
		{}

		int GetLine() const
		{
			return m_line;
		}

		int GetColumn() const
		{
			return m_column;
		}

		std::string ToString() const
		{
			return std::string{ m_begin, m_end };
		}

		friend bool operator==(const CToken& token, char c)
		{
			return (token.m_end - token.m_begin) == 1 && *(token.m_begin) == c;
		}
	};

	inline bool IsOpeningParenthesis(const CToken& token)
	{
		return
			token == TOK_CORE_OPEN ||
			token == TOK_ARR_OPEN ||
			token == TOK_TUP_OPEN;
	}

	inline bool IsClosingParenthesis(const CToken& token)
	{
		return
			token == TOK_CORE_CLOSE ||
			token == TOK_ARR_CLOSE ||
			token == TOK_TUP_CLOSE;
	}

	inline bool ParenthesisMatch(const CToken& lhs, const CToken& rhs)
	{
		return
			(lhs == TOK_CORE_OPEN && rhs == TOK_CORE_CLOSE) ||
			(lhs == TOK_ARR_OPEN &&  rhs == TOK_ARR_CLOSE) ||
			(lhs == TOK_TUP_OPEN &&  rhs == TOK_TUP_CLOSE);
	}

	inline bool DelimiterMatch(const CToken& lhs, const CToken& rhs)
	{
		return
			(lhs == TOK_STR_DELIM && rhs == TOK_STR_DELIM) ||
			(lhs == TOK_CHAR_DELIM && rhs == TOK_CHAR_DELIM);
	}
}
}
}

#endif