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

    std::vector<Token> Tokenize(StrIter current, const StrIter& last);

    std::vector<DomNode> BuildDom(const std::vector<Token>& tokens);

    std::unique_ptr<ast::AstBind> TryParsingBind(const DomNode& domNode);
    std::unique_ptr<ast::AstCompound> TryParsingCompound(const DomNode& domNode);
    std::unique_ptr<ast::AstFuncCall> TryParsingFuncCall(const DomNode& domNode);
    std::unique_ptr<ast::AstFuncDef> TryParsingFuncDef(const DomNode& domNode);
    std::unique_ptr<ast::AstLiteral> TryParsingLiteral(const DomNode& domNode);
    std::unique_ptr<ast::AstReference> TryParsingReference(const DomNode& domNode);
    std::unique_ptr<ast::AstNode> TryParsingNode(const DomNode& domNode);

    std::unique_ptr<ast::AstBind> ParseBind(const DomNode& domNode);

    bool AssertCompoundSize(const DomNode& node, int size);
    bool AssertCompoundMinSize(const DomNode& node, int minSize);

    bool ParseAtom(const DomNode& node, std::string& result);

    bool ParseLiteralBoolean(const std::string& atom, itpr::Value& result);
    bool ParseLiteralString(const std::string& atom, const SourceLocation& location, itpr::Value& result);
    bool ParseLiteralCharacter(const std::string& atom, const SourceLocation& location, itpr::Value& result);
    bool ParseLiteralInteger(const std::string& atom, itpr::Value& result);
    bool ParseLiteralReal(const std::string& atom, itpr::Value& result);

}
}
}

#endif
