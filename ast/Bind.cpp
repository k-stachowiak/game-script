#include <cassert>

#include "Bind.h"
#include "Literal.h"

#include "../itpr/Scope.h"

namespace moon {
namespace ast {

    AstBind::AstBind(
        const SourceLocation& location,
        std::string symbol, 
        std::unique_ptr<AstNode>&& expression) :
        AstNode{ location },
        m_symbol{ symbol },
        m_expression{ std::move(expression) }
    {
        assert(static_cast<bool>(m_expression));
    }

    itpr::Value AstBind::Evaluate(itpr::Scope& scope, itpr::Stack& stack) const
    {
        itpr::Value result = m_expression->Evaluate(scope, stack);
        scope.TryRegisteringBind(stack, m_symbol, result, GetLocation());
        return result;
    }

    void AstBind::GetUsedSymbols(std::vector<std::string>& symbols) const
    {
        m_expression->GetUsedSymbols(symbols);
    }

    const AstNode& AstBind::GetExpression() const
    {
        return *m_expression;
    }

    std::unique_ptr<AstNode> AstBind::MoveExpressionOut()
    {
        return std::move(m_expression);
    }
}
}
