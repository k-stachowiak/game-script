#ifndef MOON_PARSE_SEXPR_AST_PARSER_H
#define MOON_PARSE_SEXPR_AST_PARSER_H

#include "../ParserBase.h"

namespace moon {
namespace parse {
namespace sexpr {

	class CAstParser : public CParserBase
	{
	public:
		std::map<std::string, std::unique_ptr<ast::CAstNode>>
		Parse(const std::string& source) const override;
	};

}
}
}

#endif
