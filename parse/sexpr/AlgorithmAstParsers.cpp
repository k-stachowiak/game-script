#include <algorithm>
#include <iterator>

#include "Algorithm.h"

namespace moon {
namespace parse {
namespace sexpr {

	std::unique_ptr<itpr::CAstBind> TryParsingBind(const CDomNode& domNode)
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
		std::unique_ptr<itpr::CAstNode> expression;
		if (!(expression = TryParsingNode(*(current++)))) {
			return{};
		}

		return std::unique_ptr<itpr::CAstBind> { new itpr::CAstBind{ symbol, std::move(expression) }};
	}

	std::unique_ptr<itpr::CAstCompound> TryParsingCompound(const CDomNode& domNode)
	{
		// 1. Is compound.
		if (domNode.IsAtom()) {
			return{};
		}

		// 2. Is of ARRAY or TUPLE type.
		ECompoundType type;
		switch (domNode.GetType()) {
		case EDomCompoundType::CPD_ARRAY:
			type = ECompoundType::ARRAY;
			break;

		case EDomCompoundType::CPD_TUPLE:
			type = ECompoundType::TUPLE;
			break;

		default:
			return{};
		}

		// 3. Has 0 or more expressions.
		std::vector<std::unique_ptr<itpr::CAstNode>> expressions;
		for (auto it = domNode.ChildrenBegin(); it != domNode.ChildrenEnd(); ++it) {
			auto astNode = TryParsingNode(*it);
			if (!astNode) {
				return{};
			} else {
				expressions.push_back(std::move(astNode));
			}
		}

		return std::unique_ptr<itpr::CAstCompound> {
			new itpr::CAstCompound{ type, std::move(expressions) }
		};
	}

	std::unique_ptr<itpr::CAstFuncCall> TryParsingFuncCall(const CDomNode& domNode)
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
			return false;
		}

		// 3.2. Has 0 or more further children being any expression.
		std::vector<std::unique_ptr<itpr::CAstNode>> actualArgs;
		for (auto it = current; it != domNode.ChildrenEnd(); ++it) {
			auto astNode = TryParsingNode(*it);
			if (!astNode) {
				return{};
			} else {
				actualArgs.push_back(std::move(astNode));
			}
		}

		return std::unique_ptr<itpr::CAstFuncCall> {
			new itpr::CAstFuncCall{ symbol, std::move(actualArgs) }
		};
	}

	std::unique_ptr<itpr::CAstFuncDecl> TryParsingFuncDecl(const CDomNode& domNode)
	{
		// 1. Is compound CORE.
		if (!domNode.IsCompoundCore()) {
			return{};
		}

		// 2. Has 2 or more children.
		if (!AssertCompoundMinSize(domNode, 2)) {
			return{};
		}

		auto current = domNode.ChildrenBegin();

		// 3.1. 1st child is "func" keyword.
		if (*(current++) != "func") {
			return{};
		}

		// 3.2. 2nd keyword is a core compound of symbols.
		std::vector<std::string> formalArgs;

		const CDomNode& secondChild = *(current++);
		if (!secondChild.IsCompoundCore()) {
			return{};
		}

		for (auto it = secondChild.ChildrenBegin(); it != secondChild.ChildrenEnd(); ++it) {
			if (!it->IsAtom()) {
				return{};
			} else {
				formalArgs.push_back(it->GetAtom());
			}
		}

		// 3.3. Has 1 or more further expressions.
		std::vector<std::unique_ptr<itpr::CAstNode>> expressions;
		for (auto it = current; it != domNode.ChildrenEnd(); ++it) {
			auto astNode = TryParsingNode(*it);
			if (!astNode) {
				return{};
			} else {
				expressions.push_back(std::move(astNode));
			}
		}

		return std::unique_ptr<itpr::CAstFuncDecl> {
			new itpr::CAstFuncDecl{ formalArgs, std::move(expressions) }
		};
	}

	std::unique_ptr<itpr::CAstLiteral> TryParsingLiteral(const CDomNode& domNode)
	{
		// 1. Is atom.
		if (domNode.IsCompound()) {
			return{};
		}

		// 2. Can be parsed to non-compound value.
		// NOTE: The order of these assertions matters :(
		CValue value;
		if (!ParseLiteralBoolean(domNode.GetAtom(), value) &&
			!ParseLiteralString(domNode.GetAtom(), value) &&
			!ParseLiteralInteger(domNode.GetAtom(), value) &&
			!ParseLiteralReal(domNode.GetAtom(), value)) {

			return{};
		}

		return std::unique_ptr<itpr::CAstLiteral> {
			new itpr::CAstLiteral{ value }
		};
	}

	std::unique_ptr<itpr::CAstReference> TryParsingReference(const CDomNode& domNode)
	{
		// 1. Is a symbol.
		std::string symbol;
		if (!ParseAtom(domNode, symbol)) {
			return{};
		}

		return std::unique_ptr<itpr::CAstReference> {
			new itpr::CAstReference{ symbol }
		};
	}

	std::unique_ptr<itpr::CAstNode> TryParsingNode(const CDomNode& domNode)
	{
		std::unique_ptr<itpr::CAstNode> result;

		// NOTE: The order of these assertions matters :(
		if ((result = TryParsingLiteral(domNode)) ||
			(result = TryParsingReference(domNode)) ||
			(result = TryParsingBind(domNode)) ||
			(result = TryParsingFuncDecl(domNode)) ||
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
