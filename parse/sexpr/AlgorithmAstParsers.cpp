#include <algorithm>
#include <iterator>

#include "Algorithm.h"

namespace moon {
namespace parse {
namespace sexpr {

	std::unique_ptr<itpr::CAstBind> TryParsingBind(const CDomNode& domNode)
	{
		if (domNode.IsAtom()) {
			return{};
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

	std::unique_ptr<itpr::CAstCompound> TryParsingCompound(const CDomNode& domNode)
	{
		return{};
	}

	std::unique_ptr<itpr::CAstFuncCall> TryParsingFuncCall(const CDomNode& domNode)
	{
		return{};
	}

	std::unique_ptr<itpr::CAstFuncDecl> TryParsingFuncDecl(const CDomNode& domNode)
	{
		return{};
	}

	std::unique_ptr<itpr::CAstLiteral> TryParsingLiteral(const CDomNode& domNode)
	{
		return{};
	}

	std::unique_ptr<itpr::CAstReference> TryParsingReference(const CDomNode& domNode)
	{
		return{};
	}

	std::unique_ptr<itpr::CAstNode> TryParsingNode(const CDomNode& domNode)
	{
		return{};
	}

}
}
}