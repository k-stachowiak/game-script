#include <algorithm>
#include <iterator>

#include "SexprAstBuilder.h"

namespace moon {
namespace parse {

	std::unique_ptr<itpr::CAstBind> CSexprAstBuilder::TryParsingBind(const CSexprDomNode& domNode)
	{
		if (domNode.IsAtom()) {
			return {};
		}

		auto current = domNode.ChildrenBegin();
		auto last = domNode.ChildrenEnd();

		if (std::distance(current, last) != 3) {
			return{};
		}

		if (!(current++)->IsAtom("bind")) {
			return{};
		}

		if (!current->IsAtom()) {
			return{};
		}

		std::string symbol{ (current++)->GetAtom() };
		std::unique_ptr<itpr::CAstNode> expression{ TryParsingNode(*current) };
		if (!expression) {
			return{};
		}

		return std::unique_ptr<itpr::CAstBind> { new itpr::CAstBind{ symbol, std::move(expression) }};
	}

	std::unique_ptr<itpr::CAstCompound> CSexprAstBuilder::TryParsingCompound(const CSexprDomNode& domNode)
	{
		return{};
	}

	std::unique_ptr<itpr::CAstFuncCall> CSexprAstBuilder::TryParsingFuncCall(const CSexprDomNode& domNode)
	{
		return{};
	}

	std::unique_ptr<itpr::CAstFuncDecl> CSexprAstBuilder::TryParsingFuncDecl(const CSexprDomNode& domNode)
	{
		return{};
	}

	std::unique_ptr<itpr::CAstLiteral> CSexprAstBuilder::TryParsingLiteral(const CSexprDomNode& domNode)
	{
		return{};
	}

	std::unique_ptr<itpr::CAstReference> CSexprAstBuilder::TryParsingReference(const CSexprDomNode& domNode)
	{
		return{};
	}

	std::unique_ptr<itpr::CAstNode> CSexprAstBuilder::TryParsingNode(const CSexprDomNode& domNode)
	{
		return{};
	}

}
}
