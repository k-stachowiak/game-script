#include "AstParser.h"

#include "../../itpr/AstFuncDecl.h"

#include "../StrIter.h"
#include "Token.h"
#include "DomNode.h"
#include "Algorithm.h"

namespace moon {
namespace parse {
namespace sexpr {

	std::unique_ptr<itpr::CScope> CAstParser::Parse(
		const std::string& source,
		itpr::CScope* parentScope) const
	{
		std::vector<CToken> tokens = Tokenize(CStrIter::Begin(source), CStrIter::End(source));
		std::vector<CDomNode> domNodes = BuildDom(tokens);
		std::unique_ptr<itpr::CScope> result{ new itpr::CScope{ parentScope } };
		for (const CDomNode& domNode : domNodes) {
			std::unique_ptr<itpr::CAstBind> bind = TryParsingBind(domNode);
			if (!bind) {
				return{};
			} else {
				result->RegisterBind(std::move(bind));
			}
		}

		return result;
	}

}
}
}
