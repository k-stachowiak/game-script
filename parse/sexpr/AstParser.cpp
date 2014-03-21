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
		return{};
	}

}
}
}