#include "SexprParser.h"

#include "../itpr/AstFuncDecl.h"

#include "StrIter.h"
#include "SexprToken.h"
#include "SexprTokenizer.h"
#include "SexprDomNode.h"
#include "SexprDomBuilder.h"

namespace moon {
namespace parse {

	std::unique_ptr<itpr::CScope> CSexprParser::Parse(const std::string& source)
	{
		 std::vector<CSexprToken> tokens = CSexprTokenizer::Tokenize(CStrIter::Begin(source), CStrIter::End(source));
		 std::vector<CSexprDomNode> domNodes = CSexprDomBuilder::BuildDom(tokens);
		 return{};
	}

}
}
