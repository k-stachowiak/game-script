#include "AstReference.h"
#include "AstBind.h"

#include "../except/Ast.h"
#include "Scope.h"

namespace moon {
namespace itpr {

	CAstReference::CAstReference(const CSourceLocation& location, std::string symbol) :
		CAstNode{ location },
		m_symbol{ symbol }
	{}

	CValue CAstReference::Evaluate(CScope& scope) const
	{
		const auto bind = scope.GetBind(m_symbol);
		if (bind->TryGettingFunction() != nullptr) {
			throw except::ExAst::ReferenceToFunctionEvaluated{ GetLocation() };
		} else {
			return bind->TryGettingNonFunction()->Evaluate(scope);
		}
	}

}
}
