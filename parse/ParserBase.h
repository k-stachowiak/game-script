#ifndef MOON_PARSE_PARSER_BASE_H
#define MOON_PARSE_PARSER_BASE_H

#include <string>
#include <map>
#include <memory>

#include "../itpr/AstBind.h"

namespace moon {
namespace parse {

	class CParserBase {
	protected:
		static std::pair<std::string, std::unique_ptr<itpr::CAstNode>> t_StripBind(
				std::unique_ptr<itpr::CAstBind>&& bind)
		{
			return std::make_pair(bind->GetSymbol(), bind->TakeOverExpression());
		}

	public:
		virtual ~CParserBase() {}

		virtual std::map<std::string, std::unique_ptr<itpr::CAstNode>>
		Parse(const std::string& source) const = 0;
	};

}
}

#endif
