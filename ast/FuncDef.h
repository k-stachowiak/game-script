#ifndef MOON_AST_FUNC_DEF_H
#define MOON_AST_FUNC_DEF_H

#include <vector>
#include <string>
#include <memory>

#include "Function.h"

namespace moon {
namespace ast {

    class AstFuncDef : public AstFunction {
        const std::vector<std::unique_ptr<AstNode>> m_expressions;

    public:
        AstFuncDef(
            const SourceLocation& location,
            std::vector<std::string> formalArgs,
            std::vector<SourceLocation> argLocations,
            std::vector<std::unique_ptr<AstNode>>&& expressions);

        itpr::Value Execute(itpr::Scope& scope, itpr::Stack& stack) const override;
        void GetUsedSymbols(std::vector<std::string>& symbols) const override;
    };

}
}

#endif
