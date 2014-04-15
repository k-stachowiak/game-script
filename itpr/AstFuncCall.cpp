#include <cassert>

#include "AstFuncCall.h"

#include "Algorithm.h"
#include "AstFunction.h"
#include "AstLiteral.h"
#include "Scope.h"
#include "Exceptions.h"

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

	CValue CAstFuncCall::Evaluate(std::shared_ptr<CScope> scope, CStack& stack) const
	{
		std::vector<CValue> argValues;
		for (const auto& arg : m_actualArgs) {
			argValues.push_back(arg->Evaluate(scope, stack));
		}

		return ::moon::itpr::CallFunction(m_symbol, argValues, GetLocation(), scope, stack);
	}

}
}
