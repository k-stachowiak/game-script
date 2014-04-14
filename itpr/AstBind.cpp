#include <cassert>

#include "AstBind.h"
#include "AstLiteral.h"

#include "Scope.h"

namespace moon {
namespace itpr {

	CAstBind::CAstBind(
		const CSourceLocation& location,
		std::string symbol, 
		std::unique_ptr<CAstNode>&& expression) :
		CAstNode{ location },
		m_symbol{ symbol },
		m_expression{ std::move(expression) }
	{
		assert((bool)m_expression);
	}

	CValue CAstBind::Evaluate(std::shared_ptr<CScope> scope, CStack& stack) const
	{
		CValue result = m_expression->Evaluate(scope, stack);
		scope->TryRegisteringBind(GetLocation(), stack, m_symbol, result);
		return result;
	}

	const CAstNode& CAstBind::GetExpression() const
	{
		return *m_expression;
	}

	// TODO: Is this still used?
	std::unique_ptr<CAstNode> CAstBind::TakeOverExpression()
	{
		return std::move(m_expression);
	}
}
}
