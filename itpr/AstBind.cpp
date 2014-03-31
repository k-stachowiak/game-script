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
	{}

	CValue CAstBind::Evaluate(CScope& scope, CStack& stack) const
	{
		CValue result = m_expression->Evaluate(scope, stack);

		scope.RegisterBind(
			GetLocation(),
			m_symbol,
			std::unique_ptr<CAstNode> {
				new CAstLiteral{ m_expression->GetLocation(), result }
			});

		return result;
	}

	const CAstFunction* CAstBind::TryGettingFunction() const
	{
		return dynamic_cast<const CAstFunction*>(m_expression.get());
	}

	const CAstNode* CAstBind::TryGettingNonFunction() const
	{
		if (TryGettingFunction()) {
			return nullptr;
		} else {
			return m_expression.get();
		}
	}

	std::unique_ptr<CAstNode> CAstBind::TakeOverExpression()
	{
		return std::move(m_expression);
	}
}
}
