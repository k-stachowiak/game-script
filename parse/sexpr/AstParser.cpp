#include "AstParser.h"

#include "../../itpr/AstFuncDecl.h"

#include "../StrIter.h"
#include "Token.h"
#include "DomNode.h"
#include "Algorithm.h"

namespace moon {
namespace parse {
namespace sexpr {

	std::unique_ptr<itpr::CScope> CAstParser::Parse(const std::string& source)
	{
		std::vector<CToken> tokens = Tokenize(CStrIter::Begin(source), CStrIter::End(source));
		std::vector<CDomNode> domNodes = BuildDom(tokens);

		std::vector<std::unique_ptr<itpr::CAstBind>> binds;
		for (const CDomNode& domNode : domNodes) {
			std::unique_ptr<itpr::CAstBind> bind = TryParsingBind(domNode);
			if (!bind) {
				return{};
			} else {
				binds.push_back(std::move(bind));
			}
		}

		return{};
	}

}
}
}