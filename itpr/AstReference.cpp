#include "AstReference.h"
#include "AstBind.h"

#include "Exceptions.h"
#include "Scope.h"

namespace moon {
namespace itpr {

	CAstReference::CAstReference(const CSourceLocation& location, std::string symbol) :
		CAstNode{ location },
		m_symbol{ symbol }
	{}

	CValue CAstReference::Evaluate(std::shared_ptr<CScope> scope, CStack& stack) const
	{
		const auto bind = scope->GetBind(m_symbol);
		return bind->GetExpression().Evaluate(scope, stack);
	}

}
}
