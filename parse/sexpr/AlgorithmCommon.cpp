#include "Algorithm.h"

namespace moon {
namespace parse {
namespace sexpr {

	bool AssertCompoundSize(const CDomNode& node, int size)
	{
		return
			node.IsCompound() &&
			std::distance(node.ChildrenBegin(), node.ChildrenEnd()) == size;
	}

	bool AssertCompoundMinSize(const CDomNode& node, int minSize)
	{
		return
			node.IsCompound() &&
			std::distance(node.ChildrenBegin(), node.ChildrenEnd()) >= minSize;
	}

	bool ParseAtom(const CDomNode& node, std::string& result)
	{
		if (!node.IsAtom()) {
			return false;
		}

		result = node.GetAtom();
		return true;
	}
}
}
}
