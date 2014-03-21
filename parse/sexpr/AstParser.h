#ifndef MOON_PARSE_SEXPR_AST_PARSER_H
#define MOON_PARSE_SEXPR_AST_PARSER_H

#include "../ParserBase.h"

namespace moon {
namespace parse {
namespace sexpr {

	class CAstParser : public CParserBase
	{
	public:
		static std::unique_ptr<itpr::CScope> Parse(const std::string& source);
	};

}
}
}

#endif