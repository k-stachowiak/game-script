#ifndef MOON_AST_LITERAL_H
#define MOON_AST_LITERAL_H

#include "Node.h"

namespace moon {
namespace ast {

    class AstLiteral : public AstNode {
        const itpr::Value m_value;

    public:
        AstLiteral(const SourceLocation& location, itpr::Value value);
        itpr::Value Evaluate(itpr::Scope&, itpr::Stack&) const override;
        void GetUsedSymbols(std::vector<std::string>&) const override {}
    };

}
}

#endif
