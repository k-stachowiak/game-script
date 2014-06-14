#include "Algorithm.h"

namespace moon {
namespace parse {
namespace sexpr {

    bool AssertCompoundSize(const DomNode& node, int size)
    {
        return
            node.IsCompound() &&
            std::distance(node.ChildrenBegin(), node.ChildrenEnd()) == size;
    }

    bool AssertCompoundMinSize(const DomNode& node, int minSize)
    {
        return
            node.IsCompound() &&
            std::distance(node.ChildrenBegin(), node.ChildrenEnd()) >= minSize;
    }

    bool ParseAtom(const DomNode& node, std::string& result)
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
