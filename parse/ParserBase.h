#ifndef MOON_PARSE_PARSER_BASE_H
#define MOON_PARSE_PARSER_BASE_H

#include <string>
#include <memory>

#include "../itpr/Scope.h"

namespace moon {
namespace parse {

	class CParserBase {
	public:
		virtual ~CParserBase() {}
		virtual std::shared_ptr<itpr::CScope> Parse(
			const std::string& source,
			const std::shared_ptr<itpr::CScope>& parentScope) const = 0;
	};

}
}

#endif
