#include "AstParser.h"

#include "../../ast/FuncDef.h"
#include "../../itpr/Stack.h"

#include "../StrIter.h"
#include "Token.h"
#include "DomNode.h"
#include "Algorithm.h"

namespace moon {
namespace parse {
namespace sexpr {

	std::map<std::string, std::unique_ptr<ast::CAstNode>>
	CAstParser::Parse(const std::string& source) const
	{
		std::vector<CToken> tokens = Tokenize(CStrIter::Begin(source), CStrIter::End(source));
		std::vector<CDomNode> domNodes = BuildDom(tokens);
		std::map<std::string, std::unique_ptr<ast::CAstNode>> result;
		for (const CDomNode& domNode : domNodes) {
			auto bind = ParseBind(domNode);
			auto pair = t_StripBind(std::move(bind));
			result.insert(std::move(pair));
		}
		return result;
	}

}
}
}
