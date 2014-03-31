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

	CValue CAstFuncCall::Evaluate(CScope& scope, CStack& stack) const
	{
		std::vector<CValue> argValues;
		for (const auto& arg : m_actualArgs) {
			argValues.push_back(arg->Evaluate(scope, stack));
		}
		return scope.CallFunction(stack, GetLocation(), m_symbol, argValues);
	}

}
}
