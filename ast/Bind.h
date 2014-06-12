#ifndef MOON_AST_BIND_H
#define MOON_AST_BIND_H

#include <memory>

#include "../common/SourceLocation.h"
#include "Node.h"
#include "Function.h"

namespace moon {
namespace ast {
        
    class AstBind : public AstNode {
        const std::string m_symbol;
        std::unique_ptr<AstNode> m_expression;

    public:
        AstBind(
            const SourceLocation& location, 
            std::string symbol, 
            std::unique_ptr<AstNode>&& expression);

        itpr::Value Evaluate(itpr::Scope& scope, itpr::Stack& stack) const override;
        void GetUsedSymbols(std::vector<std::string>& symbols) const override;
        const std::string& GetSymbol() const { return m_symbol; }
        const AstNode& GetExpression() const;
        std::unique_ptr<AstNode> TakeOverExpression();
    };

}
}

#endif
