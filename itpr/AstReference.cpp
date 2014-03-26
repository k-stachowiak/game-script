#include "AstReference.h"
#include "AstBind.h"

#include "../except/Ast.h"
#include "Scope.h"

namespace moon {
namespace itpr {

	CAstReference::CAstReference(int line, int column, std::string symbol) :
		CAstNode{ line, column },
		m_symbol{ symbol }
	{}

	CValue CAstReference::Evaluate(CScope& scope) const
	{
		const auto bind = scope.GetBind(m_symbol);
		if (bind->TryGettingFuncDecl() != nullptr) {
			throw except::ExAst::ReferenceToFunctionEvaluated{ GetLine(), GetColumn() };
		} else {
			return bind->TryGettingNonFuncDecl()->Evaluate(scope);
		}
	}

}
}
