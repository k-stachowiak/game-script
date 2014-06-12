#include "FuncDef.h"

namespace moon {
namespace ast {

    AstFuncDef::AstFuncDef(
        const SourceLocation& location,
        std::vector<std::string> formalArgs,
        std::vector<SourceLocation> argLocations,
        std::vector<std::unique_ptr<AstNode>>&& expressions) :
        AstFunction{ location, formalArgs, argLocations },
        m_expressions{ std::move(expressions) }
    {}

    itpr::Value AstFuncDef::Execute(itpr::Scope& scope, itpr::Stack& stack) const
    {
        itpr::Value result;
        for (const auto& expression : m_expressions) {
            result = expression->Evaluate(scope, stack);
        }
        return result;
    }

    void AstFuncDef::GetUsedSymbols(std::vector<std::string>& symbols) const
    {
        for (const auto& expression : m_expressions) {
            expression->GetUsedSymbols(symbols);
        }
    }

}
}
