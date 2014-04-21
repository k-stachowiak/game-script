#include <cassert>

#include "Bind.h"
#include "Literal.h"

#include "../itpr/Scope.h"

namespace moon {
namespace ast {

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

	CValue CAstBind::Evaluate(itpr::CScope& scope, itpr::CStack& stack) const
	{
		CValue result = m_expression->Evaluate(scope, stack);
		scope.TryRegisteringBind(stack, m_symbol, result, GetLocation());
		return result;
	}

	void CAstBind::GetUsedSymbols(std::vector<std::string>& symbols) const
	{
		m_expression->GetUsedSymbols(symbols);
	}

	const CAstNode& CAstBind::GetExpression() const
	{
		return *m_expression;
	}

	std::unique_ptr<CAstNode> CAstBind::TakeOverExpression()
	{
		return std::move(m_expression);
	}
}
}
