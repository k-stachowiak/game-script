#include "Algorithm.h"

#include "Exceptions.h"

namespace moon {
namespace itpr {

	CValue CallFunction(
		CScope& scope,
		CStack& stack,
		const CSourceLocation& location,
		const std::string& symbol,
		const std::vector<CValue>& argValues)
	{
		// TODO: Fix the wtf chaos below!

		CValue functionValue = scope.GetValue(symbol);
		if (!IsFunction(functionValue)) {
			throw ExScopeSymbolIsNotFunction{ location, stack };
		}

		// 1. Passed more than can be applied - error.
		// -------------------------------------------

		if (argValues.size() > functionValue.GetFuncArity()) {
			throw ExScopeFormalActualArgCountMismatch{ location, stack };
		}

		// 2. Passed less than needed - curry on ;-).
		// ------------------------------------------

		auto& functionDef = functionValue.GetFuncDef();
		auto applArgs = functionValue.GetAppliedArgs();
		std::copy(begin(argValues), end(argValues), std::back_inserter(applArgs));

		std::vector<std::string> capNames;
		std::vector<CValue> capValues;
		std::vector<CSourceLocation> capLocations;
		functionValue.GetFuncCaptures(capNames, capValues, capLocations);
		unsigned capCount = capNames.size();

		if (argValues.size() < functionValue.GetFuncArity()) {
			return CValue::MakeFunction(&functionDef, capNames, capValues, capLocations, applArgs);
		}

		// 3. Passed the exact amount needed for the execution.
		// ----------------------------------------------------

		// 3.1. Analyze args.
		const std::vector<std::string>& argNames = functionDef.GetFormalArgs();
		const std::vector<CSourceLocation>& argLocations = functionDef.GetArgLocations();
		assert(argNames.size() == argLocations.size());
		unsigned argsCount = argNames.size();

		// 3.2. Build local scope for the function.
		CLocalScope funcScope{ scope.GetGlobalScope() };
		for (unsigned i = 0; i < argsCount; ++i) {
			funcScope.TryRegisteringBind(argLocations[i], stack, argNames[i], applArgs[i]);
		}
		for (unsigned i = 0; i < capCount; ++i) {
			funcScope.TryRegisteringBind(
				capLocations[i],
				stack,
				capNames[i],
				capValues[i]);
		}

		// 3.3. Execute the function.
		stack.Push(symbol);
		CValue result = functionDef.Execute(funcScope, stack);
		stack.Pop();

		return result;
	}

}
}
