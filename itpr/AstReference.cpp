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

	CValue CAstReference::Evaluate(CScope& scope, CStack& stack) const
	{
		// TODO: The call below may throw - test for this and all other cases like this.
		const auto bind = scope.GetBind(m_symbol);
		if (bind->TryGettingFunction() != nullptr) {
			throw ExAstReferenceToFunctionEvaluated{ GetLocation(), stack };
		} else {
			return bind->TryGettingNonFunction()->Evaluate(scope, stack);
		}
	}

}
}
