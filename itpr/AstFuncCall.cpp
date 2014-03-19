#include "AstFuncCall.h"

#include "AstFuncDecl.h"
#include "Scope.h"

namespace moon {
namespace itpr {

	CAstFuncCall::CAstFuncCall(std::string symbol, std::vector<std::unique_ptr<CAstNode>>&& actualArgs) :
		m_symbol{ symbol },
		m_actualArgs{ std::move(actualArgs) }
	{}

	CValue CAstFuncCall::Evaluate(CScope& scope) const
	{
		CAstFuncDecl& funcDecl = scope.GetFunction(m_symbol);
		std::vector<CValue> argValues;
		for (const auto& arg : m_actualArgs) {
			argValues.push_back(arg->Evaluate(scope));
		}
		return g_CallFunction(scope, funcDecl, argValues);
	}

}
}