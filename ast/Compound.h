#ifndef MOON_AST_COMPOUND_H
#define MOON_AST_COMPOUND_H

#include <memory>

#include "Node.h"

namespace moon {
namespace ast {

    class CAstCompound : public CAstNode {
        const itpr::ECompoundType m_type;
        const std::vector<std::unique_ptr<CAstNode>> m_expressions;

    public:
        CAstCompound(
            const CSourceLocation& location,
            itpr::ECompoundType type, 
            std::vector<std::unique_ptr<CAstNode>>&& expressions);

        itpr::CValue Evaluate(itpr::CScope& scope, itpr::CStack& stack) const override;
        void GetUsedSymbols(std::vector<std::string>& symbols) const override;
    };

}
}

#endif
