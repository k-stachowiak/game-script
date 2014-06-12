#include <cassert>

#include "FuncCall.h"

#include "../itpr/Algorithm.h"
#include "../itpr/Scope.h"
#include "../itpr/Exceptions.h"
#include "Function.h"
#include "Literal.h"

namespace moon {
namespace ast {

    AstFuncCall::AstFuncCall(
        const SourceLocation& location,
        std::string symbol, 
        std::vector<std::unique_ptr<AstNode>>&& actualArgs) :
        AstNode{ location },
        m_symbol{ symbol },
        m_actualArgs{ std::move(actualArgs) }
    {}

    itpr::Value AstFuncCall::Evaluate(itpr::Scope& scope, itpr::Stack& stack) const
    {
        std::vector<itpr::Value> values;
        for (const auto& arg : m_actualArgs) {
            values.push_back(arg->Evaluate(scope, stack));
        }
        return itpr::CallFunction(scope, stack, GetLocation(), m_symbol, values);
    }

    void AstFuncCall::GetUsedSymbols(std::vector<std::string>& symbols) const
    {
        symbols.push_back(m_symbol);
        for (const auto& arg : m_actualArgs) {
            arg->GetUsedSymbols(symbols);
        }
    }

}
}
