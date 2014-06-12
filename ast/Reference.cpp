#include "Reference.h"
#include "Bind.h"

#include "../itpr/Scope.h"

namespace moon {
namespace ast {

    AstReference::AstReference(const SourceLocation& location, std::string symbol) :
        AstNode{ location },
        m_symbol{ symbol }
    {}

    itpr::Value AstReference::Evaluate(itpr::Scope& scope, itpr::Stack& stack) const
    {
        return scope.GetValue(m_symbol, GetLocation(), stack);
    }

    void AstReference::GetUsedSymbols(std::vector<std::string>& symbols) const
    {
        symbols.push_back(m_symbol);
    }

}
}
