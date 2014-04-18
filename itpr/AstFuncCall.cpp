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

	CValue CAstFuncCall::Evaluate(CScope& scope, CStack& stack) const
	{
		std::vector<CValue> values;
		for (const auto& arg : m_actualArgs) {
			values.push_back(arg->Evaluate(scope, stack));
		}
		return itpr::CallFunction(scope, stack, GetLocation(), m_symbol, values);
	}

	void CAstFuncCall::GetUsedSymbols(std::vector<std::string>& symbols) const
	{
		symbols.push_back(m_symbol);
		for (const auto& arg : m_actualArgs) {
			arg->GetUsedSymbols(symbols);
		}
	}

}
}
