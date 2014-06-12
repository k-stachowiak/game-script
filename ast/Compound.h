#ifndef MOON_AST_COMPOUND_H
#define MOON_AST_COMPOUND_H

#include <memory>

#include "Node.h"

namespace moon {
namespace ast {

    class AstCompound : public AstNode {
        const itpr::CompoundType m_type;
        const std::vector<std::unique_ptr<AstNode>> m_expressions;

    public:
        AstCompound(
            const SourceLocation& location,
            itpr::CompoundType type, 
            std::vector<std::unique_ptr<AstNode>>&& expressions);

        itpr::Value Evaluate(itpr::Scope& scope, itpr::Stack& stack) const override;
        void GetUsedSymbols(std::vector<std::string>& symbols) const override;
    };

}
}

#endif
