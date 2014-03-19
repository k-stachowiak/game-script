#ifndef MOON_PARSE_SEXPR_PARSER_H
#define MOON_PARSE_SEXPR_PARSER_H

#include "Parser.h"

namespace moon {
namespace parse {

	class CSexprParser : public CParser
	{
	public:
		static std::unique_ptr<itpr::CScope> Parse(const std::string& source);
	};

}
}

#endif