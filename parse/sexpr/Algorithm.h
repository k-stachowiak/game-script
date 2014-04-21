#ifndef MOON_PARSE_SEXPR_ALGORITHM_H
#define MOON_PARSE_SEXPR_ALGORITHM_H

#include <vector>
#include <string>
#include <memory>

#include "../StrIter.h"
#include "Token.h"
#include "DomNode.h"

#include "../../ast/Node.h"
#include "../../ast/Bind.h"
#include "../../ast/Compound.h"
#include "../../ast/FuncCall.h"
#include "../../ast/FuncDef.h"
#include "../../ast/Literal.h"
#include "../../ast/Reference.h"

namespace moon {
namespace parse {
namespace sexpr {

	std::vector<CToken> Tokenize(CStrIter current, const CStrIter& last);

	std::vector<CDomNode> BuildDom(const std::vector<CToken>& tokens);

	std::unique_ptr<ast::CAstBind> TryParsingBind(const CDomNode& domNode);
	std::unique_ptr<ast::CAstCompound> TryParsingCompound(const CDomNode& domNode);
	std::unique_ptr<ast::CAstFuncCall> TryParsingFuncCall(const CDomNode& domNode);
	std::unique_ptr<ast::CAstFuncDef> TryParsingFuncDef(const CDomNode& domNode);
	std::unique_ptr<ast::CAstLiteral> TryParsingLiteral(const CDomNode& domNode);
	std::unique_ptr<ast::CAstReference> TryParsingReference(const CDomNode& domNode);
	std::unique_ptr<ast::CAstNode> TryParsingNode(const CDomNode& domNode);

	std::unique_ptr<ast::CAstBind> ParseBind(const CDomNode& domNode);

	bool AssertCompoundSize(const CDomNode& node, int size);
	bool AssertCompoundMinSize(const CDomNode& node, int minSize);

	bool ParseAtom(const CDomNode& node, std::string& result);

	bool ParseLiteralBoolean(const std::string& atom, CValue& result);
	bool ParseLiteralString(const std::string& atom, const CSourceLocation& location, CValue& result);
	bool ParseLiteralCharacter(const std::string& atom, const CSourceLocation& location, CValue& result);
	bool ParseLiteralInteger(const std::string& atom, CValue& result);
	bool ParseLiteralReal(const std::string& atom, CValue& result);

}
}
}

#endif
