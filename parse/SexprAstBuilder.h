#ifndef MOON_PARSE_SEXPR_AST_BUILDER_H
#define MOON_PARSE_SEXPR_AST_BUILDER_H

#include <memory>

#include "SexprDomNode.h"
#include "../itpr/AstNode.h"
#include "../itpr/AstBind.h"
#include "../itpr/AstCompound.h"
#include "../itpr/AstFuncCall.h"
#include "../itpr/AstFuncDecl.h"
#include "../itpr/AstLiteral.h"
#include "../itpr/AstReference.h"

namespace moon {
namespace parse {

	class CSexprAstBuilder
	{
	public:
		static std::unique_ptr<itpr::CAstBind> TryParsingBind(const CSexprDomNode& domNode);
		static std::unique_ptr<itpr::CAstCompound> TryParsingCompound(const CSexprDomNode& domNode);
		static std::unique_ptr<itpr::CAstFuncCall> TryParsingFuncCall(const CSexprDomNode& domNode);
		static std::unique_ptr<itpr::CAstFuncDecl> TryParsingFuncDecl(const CSexprDomNode& domNode);
		static std::unique_ptr<itpr::CAstLiteral> TryParsingLiteral(const CSexprDomNode& domNode);
		static std::unique_ptr<itpr::CAstReference> TryParsingReference(const CSexprDomNode& domNode);

		static std::unique_ptr<itpr::CAstNode> TryParsingNode(const CSexprDomNode& domNode);
	};

}
}


#endif