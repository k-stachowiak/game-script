#ifndef MOON_PARSE_SEXPR_DOM_BUILDER_H
#define MOON_PARSE_SEXPR_DOM_BUILDER_H

#include <vector>

#include "SexprToken.h"
#include "SexprDomNode.h"

namespace moon {
namespace parse {

	class CSexprDomBuilder
	{
	public:
		static std::vector<CSexprDomNode> BuildDom(const std::vector<CSexprToken>& tokens);
	};

}
}


#endif