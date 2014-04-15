#include "Algorithm.h"

#include "../API/Value.h"
#include "../common/SourceLocation.h"

#include "Exceptions.h"
#include "Scope.h"
#include "Stack.h"

namespace moon {
namespace itpr {

	CValue CallFunction(
			std::string symbol,
			const std::vector<CValue>& argValues,
			const CSourceLocation& location,
			std::shared_ptr<CScope> scope,
			CStack& stack)
	{
		// TODO: Generalize the logic below as an "algorithm"
		// and use it from here and from as well as from the engine.

		// 1. Acquire the function from the scope.
		// =======================================
		CValue functionValue = scope->GetValue(symbol);
		if (!IsFunction(functionValue)) {
			throw ExScopeSymbolIsNotFunction{ location, stack };
		}

		// 2. Analyze the arguments case.
		// ==============================

		// 2.1. Passed more than can be applied - error.
		// ---------------------------------------------
		if (argValues.size() > functionValue.GetFuncArity()) {
			throw ExScopeFormalActualArgCountMismatch{ location, stack };
		}

		// 2.2. Passed less than needed - curry on ;-).
		// --------------------------------------------
		auto functionDef = functionValue.GetFuncDef();
		auto functionScope = functionValue.GetFuncScope();
		auto applArgs = functionValue.GetAppliedArgs();
		std::copy(begin(argValues), end(argValues), std::back_inserter(applArgs));

		if (argValues.size() < functionValue.GetFuncArity()) {
			return CValue::MakeFunction(functionDef, functionScope, applArgs);
		}

		// 2.3. Passed the exact amount needed for the execution.
		// ------------------------------------------------------

		// 2.3.1. Analyze args.
		const auto& argNames = functionDef->GetFormalArgs();
		const auto& argLocations = functionDef->GetArgLocations();
		assert(argNames.size() == argLocations.size());
		unsigned argsCount = argNames.size();

		// 2.3.2. Build local scope for the function.
		auto funcScope = std::make_shared<CScope>(functionScope);
		for (unsigned i = 0; i < argsCount; ++i) {
			funcScope->TryRegisteringBind(argLocations[i], stack, argNames[i], applArgs[i]);
		}

		// 2.3.3. Execute the function.
		stack.Push(symbol);
		CValue result = functionDef->Execute(funcScope, stack);
		stack.Pop();

		return result;
	}

}
}
