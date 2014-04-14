#include <cassert>

#include "AstFuncCall.h"

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
		// 1. Acquire the function from the scope.
		// =======================================
		CValue functionValue = scope->GetValue(m_symbol);
		if (!IsFunction(functionValue)) {
			throw ExScopeSymbolIsNotFunction{ GetLocation(), stack };
		}

		// 2. Analyze the arguments case.
		// ==============================

		// 2.1. Evaluate the arguments.
		std::vector<CValue> argValues;
		for (const auto& arg : m_actualArgs) {
			argValues.push_back(arg->Evaluate(scope, stack));
		}

		// 2.2. Passed more than can be applied - error.
		// ---------------------------------------------
		if (argValues.size() > functionValue.GetFuncArity()) {
			throw ExScopeFormalActualArgCountMismatch{
				GetLocation(),
				stack
			};
		}

		// 2.3. Passed less than needed - curry on ;-).
		// --------------------------------------------
		auto functionDef = functionValue.GetFuncDef();
		auto functionScope = functionValue.GetFuncScope();
		auto applArgs = functionValue.GetAppliedArgs();
		std::copy(begin(argValues), end(argValues), std::back_inserter(applArgs));

		if (argValues.size() < functionValue.GetFuncArity()) {
			return CValue::MakeFunction(functionDef, functionScope, applArgs);
		}

		// 2.4. Passed the exact amount needed for the execution.
		// ------------------------------------------------------

		// 2.4.1. Analyze args.
		const auto& argNames = functionDef->GetFormalArgs();
		const auto& argLocations = functionDef->GetArgLocations();
		assert(argNames.size() == argLocations.size());
		unsigned argsCount = argNames.size();

		// 2.4.2. Build local scope for the function.
		auto funcScope = std::make_shared<CScope>(functionScope);
		for (unsigned i = 0; i < argsCount; ++i) {
			funcScope->TryRegisteringBind(argLocations[i], stack, argNames[i], applArgs[i]);
		}

		// 2.4.3. Execute the function.
		stack.Push(m_symbol);
		CValue result = functionDef->Execute(funcScope, stack);
		stack.Pop();

		return result;
	}

}
}
