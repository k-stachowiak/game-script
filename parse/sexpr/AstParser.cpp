#include "AstParser.h"

#include "../../itpr/AstFuncDef.h"
#include "../../itpr/Stack.h"

#include "../StrIter.h"
#include "Token.h"
#include "DomNode.h"
#include "Algorithm.h"

namespace moon {
namespace parse {
namespace sexpr {

	std::shared_ptr<itpr::CScope> CAstParser::Parse(
		const std::string& source,
		const std::shared_ptr<itpr::CScope>& parentScope) const
	{
		itpr::CStack stack; // TODO: Try removing this stack. Maybe pass it from the engine?
		std::vector<CToken> tokens = Tokenize(CStrIter::Begin(source), CStrIter::End(source));
		std::vector<CDomNode> domNodes = BuildDom(tokens);
		auto result = std::make_shared<itpr::CScope>(parentScope);
		for (const CDomNode& domNode : domNodes) {
			std::unique_ptr<itpr::CAstBind> bind = ParseBind(domNode);
			result->RegisterBind(
					bind->GetSymbol(),
					bind->Evaluate(result, stack));
		}

		return result;
	}

}
}
}
