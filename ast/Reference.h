#ifndef MOON_AST_REFERENCE_H
#define MOON_AST_REFERENCE_H

#include "Node.h"

namespace moon {
namespace ast {

    class AstReference : public AstNode {
        const std::string m_symbol;

    public:
        AstReference(const SourceLocation& location, std::string symbol);
        itpr::Value Evaluate(itpr::Scope& scope, itpr::Stack& stack) const override;
        void GetUsedSymbols(std::vector<std::string>& symbols) const override;
    };

}
}

#endif
