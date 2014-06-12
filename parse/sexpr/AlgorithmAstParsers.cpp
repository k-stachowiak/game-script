#include <algorithm>
#include <iterator>

#include "Exceptions.h"
#include "Exceptions.h"
#include "Algorithm.h"

namespace moon {
namespace parse {
namespace sexpr {

    std::unique_ptr<ast::AstBind> TryParsingBind(const DomNode& domNode)
    {
        // 1. Is compound CORE
        if (!domNode.IsCompoundCore()) {
            return{};
        }

        // 2. Has 3 children
        if (!AssertCompoundSize(domNode, 3)) {
            return{};
        }

        auto current = domNode.ChildrenBegin();

        // 3.1. 1st child is bind keyword.
        if (*(current++) != "bind") {
            return{};
        }

        // 3.2. 2nd child is symbol.
        std::string symbol;
        if (!ParseAtom(*(current++), symbol)) {
            return{};
        }

        // 3.3 3rd child is any expression.
        std::unique_ptr<ast::AstNode> expression;
        if (!(expression = TryParsingNode(*(current++)))) {
            return{};
        }

        return std::unique_ptr<ast::AstBind> { 
            new ast::AstBind{ domNode.GetLocation(), symbol, std::move(expression) }
        };
    }

    std::unique_ptr<ast::AstBind> ParseBind(const DomNode& domNode)
    {
        auto result = TryParsingBind(domNode);
        if (result) {
            return result;
        } else {
            throw ExAstBindParsingFailed{ domNode.GetLocation() };
        }
    }

    std::unique_ptr<ast::AstCompound> TryParsingCompound(const DomNode& domNode)
    {
        // 1. Is compound.
        if (domNode.IsAtom()) {
            return{};
        }

        // 2. Is of ARRAY or TUPLE type.
        itpr::CompoundType type;
        switch (domNode.GetType()) {
        case DomCompoundType::ARRAY:
            type = itpr::CompoundType::ARRAY;
            break;

        case DomCompoundType::TUPLE:
            type = itpr::CompoundType::TUPLE;
            break;

        default:
            return{};
        }

        // 3. Has 0 or more expressions.
        std::vector<std::unique_ptr<ast::AstNode>> expressions;
        for (auto it = domNode.ChildrenBegin(); it != domNode.ChildrenEnd(); ++it) {
            auto astNode = TryParsingNode(*it);
            if (!astNode) {
                return{};
            } else {
                expressions.push_back(std::move(astNode));
            }
        }

        return std::unique_ptr<ast::AstCompound> {
            new ast::AstCompound{ domNode.GetLocation(), type, std::move(expressions) }
        };
    }

    std::unique_ptr<ast::AstFuncCall> TryParsingFuncCall(const DomNode& domNode)
    {
        // 1. Is compound CORE.
        if (!domNode.IsCompoundCore()) {
            return{};
        }

        // 2. Has 1 or more children.
        if (!AssertCompoundMinSize(domNode, 1)) {
            return{};
        }

        auto current = domNode.ChildrenBegin();

        // 3.1. 1st child is symbol.
        std::string symbol;
        if (!ParseAtom(*(current++), symbol)) {
            return{};
        }

        // 3.2. Has 0 or more further children being any expression.
        std::vector<std::unique_ptr<ast::AstNode>> actualArgs;
        for (auto it = current; it != domNode.ChildrenEnd(); ++it) {
            auto astNode = TryParsingNode(*it);
            if (!astNode) {
                return{};
            } else {
                actualArgs.push_back(std::move(astNode));
            }
        }

        return std::unique_ptr<ast::AstFuncCall> {
            new ast::AstFuncCall{ domNode.GetLocation(), symbol, std::move(actualArgs) }
        };
    }

    std::unique_ptr<ast::AstFuncDef> TryParsingFuncDef(const DomNode& domNode)
    {
        // 1. Is compound CORE.
        if (!domNode.IsCompoundCore()) {
            return{};
        }

        // 2. Has 3 or more children.
        if (!AssertCompoundMinSize(domNode, 3)) {
            return{};
        }

        auto current = domNode.ChildrenBegin();

        // 3.1. 1st child is "func" keyword.
        if (*(current++) != "func") {
            return{};
        }

        // 3.2. 2nd keyword is a core compound of symbols.
        std::vector<std::string> formalArgs;
        std::vector<SourceLocation> argLocations;

        const DomNode& secondChild = *(current++);
        if (!secondChild.IsCompoundCore()) {
            return{};
        }

        for (auto it = secondChild.ChildrenBegin(); it != secondChild.ChildrenEnd(); ++it) {
            if (!it->IsAtom()) {
                return{};
            } else {
                formalArgs.push_back(it->GetAtom());
                argLocations.push_back(it->GetLocation());
            }
        }

        // 3.3. Has 1 or more further expressions.
        std::vector<std::unique_ptr<ast::AstNode>> expressions;
        for (auto it = current; it != domNode.ChildrenEnd(); ++it) {
            auto astNode = TryParsingNode(*it);
            if (!astNode) {
                return{};
            } else {
                expressions.push_back(std::move(astNode));
            }
        }

        return std::unique_ptr<ast::AstFuncDef> {
            new ast::AstFuncDef{
                domNode.GetLocation(),
                formalArgs,
                argLocations,
                std::move(expressions) }
        };
    }

    std::unique_ptr<ast::AstLiteral> TryParsingLiteral(const DomNode& domNode)
    {
        // 1. Is atom.
        if (domNode.IsCompound()) {
            return{};
        }

        // 2. Can be parsed to non-compound value.
        // NOTE: The order of these assertions matters :(
        itpr::Value value;
        if (!ParseLiteralBoolean(domNode.GetAtom(), value) &&
            !ParseLiteralString(domNode.GetAtom(), domNode.GetLocation(), value) &&
            !ParseLiteralCharacter(domNode.GetAtom(), domNode.GetLocation(), value) &&
            !ParseLiteralInteger(domNode.GetAtom(), value) &&
            !ParseLiteralReal(domNode.GetAtom(), value)) {

            return{};
        }

        return std::unique_ptr<ast::AstLiteral> {
            new ast::AstLiteral{ domNode.GetLocation(), value }
        };
    }

    std::unique_ptr<ast::AstReference> TryParsingReference(const DomNode& domNode)
    {
        // 1. Is a symbol.
        std::string symbol;
        if (!ParseAtom(domNode, symbol)) {
            return{};
        }

        return std::unique_ptr<ast::AstReference> {
            new ast::AstReference{ domNode.GetLocation(), symbol }
        };
    }

    std::unique_ptr<ast::AstNode> TryParsingNode(const DomNode& domNode)
    {
        std::unique_ptr<ast::AstNode> result;

        // NOTE: The order of these assertions matters :(
        if ((result = TryParsingLiteral(domNode)) ||
            (result = TryParsingReference(domNode)) ||
            (result = TryParsingBind(domNode)) ||
            (result = TryParsingFuncDef(domNode)) ||
            (result = TryParsingFuncCall(domNode)) ||
            (result = TryParsingCompound(domNode))) {
            return result;
        } else {
            return{};
        }
    }

}
}
}
