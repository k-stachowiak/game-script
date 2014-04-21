#include <algorithm>
#include <iterator>

#include "Exceptions.h"
#include "Exceptions.h"
#include "Algorithm.h"

namespace moon {
namespace parse {
namespace sexpr {

	std::unique_ptr<ast::CAstBind> TryParsingBind(const CDomNode& domNode)
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
		std::unique_ptr<ast::CAstNode> expression;
		if (!(expression = TryParsingNode(*(current++)))) {
			return{};
		}

		return std::unique_ptr<ast::CAstBind> { 
			new ast::CAstBind{ domNode.GetLocation(), symbol, std::move(expression) }
		};
	}

	std::unique_ptr<ast::CAstBind> ParseBind(const CDomNode& domNode)
	{
		auto result = TryParsingBind(domNode);
		if (result) {
			return result;
		} else {
			throw ExAstBindParsingFailed{ domNode.GetLocation() };
		}
	}

	std::unique_ptr<ast::CAstCompound> TryParsingCompound(const CDomNode& domNode)
	{
		// 1. Is compound.
		if (domNode.IsAtom()) {
			return{};
		}

		// 2. Is of ARRAY or TUPLE type.
		ECompoundType type;
		switch (domNode.GetType()) {
		case EDomCompoundType::ARRAY:
			type = ECompoundType::ARRAY;
			break;

		case EDomCompoundType::TUPLE:
			type = ECompoundType::TUPLE;
			break;

		default:
			return{};
		}

		// 3. Has 0 or more expressions.
		std::vector<std::unique_ptr<ast::CAstNode>> expressions;
		for (auto it = domNode.ChildrenBegin(); it != domNode.ChildrenEnd(); ++it) {
			auto astNode = TryParsingNode(*it);
			if (!astNode) {
				return{};
			} else {
				expressions.push_back(std::move(astNode));
			}
		}

		return std::unique_ptr<ast::CAstCompound> {
			new ast::CAstCompound{ domNode.GetLocation(), type, std::move(expressions) }
		};
	}

	std::unique_ptr<ast::CAstFuncCall> TryParsingFuncCall(const CDomNode& domNode)
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
		std::vector<std::unique_ptr<ast::CAstNode>> actualArgs;
		for (auto it = current; it != domNode.ChildrenEnd(); ++it) {
			auto astNode = TryParsingNode(*it);
			if (!astNode) {
				return{};
			} else {
				actualArgs.push_back(std::move(astNode));
			}
		}

		return std::unique_ptr<ast::CAstFuncCall> {
			new ast::CAstFuncCall{ domNode.GetLocation(), symbol, std::move(actualArgs) }
		};
	}

	std::unique_ptr<ast::CAstFuncDef> TryParsingFuncDef(const CDomNode& domNode)
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
		std::vector<CSourceLocation> argLocations;

		const CDomNode& secondChild = *(current++);
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
		std::vector<std::unique_ptr<ast::CAstNode>> expressions;
		for (auto it = current; it != domNode.ChildrenEnd(); ++it) {
			auto astNode = TryParsingNode(*it);
			if (!astNode) {
				return{};
			} else {
				expressions.push_back(std::move(astNode));
			}
		}

		return std::unique_ptr<ast::CAstFuncDef> {
			new ast::CAstFuncDef{
				domNode.GetLocation(),
				formalArgs,
				argLocations,
				std::move(expressions) }
		};
	}

	std::unique_ptr<ast::CAstLiteral> TryParsingLiteral(const CDomNode& domNode)
	{
		// 1. Is atom.
		if (domNode.IsCompound()) {
			return{};
		}

		// 2. Can be parsed to non-compound value.
		// NOTE: The order of these assertions matters :(
		CValue value;
		if (!ParseLiteralBoolean(domNode.GetAtom(), value) &&
			!ParseLiteralString(domNode.GetAtom(), domNode.GetLocation(), value) &&
			!ParseLiteralCharacter(domNode.GetAtom(), domNode.GetLocation(), value) &&
			!ParseLiteralInteger(domNode.GetAtom(), value) &&
			!ParseLiteralReal(domNode.GetAtom(), value)) {

			return{};
		}

		return std::unique_ptr<ast::CAstLiteral> {
			new ast::CAstLiteral{ domNode.GetLocation(), value }
		};
	}

	std::unique_ptr<ast::CAstReference> TryParsingReference(const CDomNode& domNode)
	{
		// 1. Is a symbol.
		std::string symbol;
		if (!ParseAtom(domNode, symbol)) {
			return{};
		}

		return std::unique_ptr<ast::CAstReference> {
			new ast::CAstReference{ domNode.GetLocation(), symbol }
		};
	}

	std::unique_ptr<ast::CAstNode> TryParsingNode(const CDomNode& domNode)
	{
		std::unique_ptr<ast::CAstNode> result;

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
