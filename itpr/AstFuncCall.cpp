#include "AstFuncCall.h"

#include "AstFuncDecl.h"
#include "Scope.h"

namespace moon {
namespace itpr {

	CAstFuncCall::CAstFuncCall(
		const CSourceLocation& location,
		std::string symbol, 
		std::vector<std::unique_ptr<CAstNode>>&& actualArgs) :
		CAstNode{ location },
		m_symbol{ symbol },
		m_actualArgs{ std::move(actualArgs) }
	{}

	// TODO: Don't build full scope list - only global <- local
	CValue CAstFuncCall::Evaluate(CScope& scope) const
	{
		std::vector<CValue> argValues;
		for (const auto& arg : m_actualArgs) {
			argValues.push_back(arg->Evaluate(scope));
		}
		return scope.CallFunction(GetLocation(), m_symbol, argValues);
	}

}
}
