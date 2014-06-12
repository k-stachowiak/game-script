#ifndef MOON_AST_FUNC_CALL_H
#define MOON_AST_FUNC_CALL_H

#include <memory>

#include "Node.h"

namespace moon {
namespace ast {

    class AstFuncCall : public AstNode {
        const std::string m_symbol;
        const std::vector<std::unique_ptr<AstNode>> m_actualArgs;

    public:
        AstFuncCall(
            const SourceLocation& location,
            std::string symbol, 
            std::vector<std::unique_ptr<AstNode>>&& actualArgs);

        itpr::Value Evaluate(itpr::Scope& scope, itpr::Stack& stack) const override;
        void GetUsedSymbols(std::vector<std::string>& symbols) const override;
    };

}
}

#endif
