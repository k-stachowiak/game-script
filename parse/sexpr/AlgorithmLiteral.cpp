#include <algorithm>
#include <sstream>

#include "Exceptions.h"
#include "Algorithm.h"

namespace moon {
namespace parse {
namespace sexpr {

	bool ParseLiteralBoolean(const std::string& atom, CValue& result)
	{
		if (atom == "true") {
			result = CValue::MakeBoolean(1);
			return true;
		} else if (atom == "false") {
			result = CValue::MakeBoolean(0);
			return true;
		} else {
			return false;
		}
	}

	bool ParseLiteralString(
		const std::string& atom, 
		const CSourceLocation& location,
		CValue& result)
	{
		char delim = TOK_STR_DELIM;

		if ((atom.front() == delim && atom.back() != delim) ||
			(atom.front() != delim && atom.back() == delim)) {
			throw ExMalformedDelimitedLiteral{ location };
		}

		unsigned length = std::distance(begin(atom), end(atom));
		if (atom.front() == delim && atom.back() == delim && length >= 2) {
			result = CValue::MakeString(atom.substr(1, length - 2));
			return true;
		} else {
			return false;
		}
	}

	bool ParseLiteralCharacter(
		const std::string& atom,
		const CSourceLocation& location,
		CValue& result)
	{
		char delim = TOK_CHAR_DELIM;

		if ((atom.front() == delim && atom.back() != delim) ||
			(atom.front() != delim && atom.back() == delim)) {
			throw ExMalformedDelimitedLiteral{ location };
		}

		unsigned length = std::distance(begin(atom), end(atom));
		if (atom.front() == delim && atom.back() == delim) {
			if ((length > 4) || (length < 3) || (length == 4 && atom.at(1) != '\\')) {
				throw ExMalformedDelimitedLiteral{ location };
			}
			result = CValue::MakeCharacter(atom.at(length - 2));
			return true;
		}
		else {
			return false;
		}
	}

	bool ParseLiteralInteger(const std::string& atom, CValue& result)
	{
		auto it = begin(atom);
		if (!atom.empty() && (isdigit(*it) || (*it) == '-' || (*it) == '+') &&
			std::all_of(it + 1, end(atom), isdigit)) {
			std::stringstream ss;
			ss << atom;
			long integer;
			ss >> integer;
			result = CValue::MakeInteger(integer);
			return true;
		} else {
			return false;
		}
	}

	bool ParseLiteralReal(const std::string& atom, CValue& result)
	{
		std::stringstream ss;
		ss << atom;
		double real;
		ss >> real;
		if (ss.eof()) {
			result = CValue::MakeReal(real);
			return true;
		} else {
			return false;
		}
	}

}
}
}
