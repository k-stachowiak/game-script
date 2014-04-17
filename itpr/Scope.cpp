#include "Scope.h"

#include <stdexcept>
#include <cassert>

#include "Exceptions.h"
#include "AstBind.h"
#include "AstLiteral.h"
#include "Stack.h"

namespace moon {
namespace itpr {

	void CScope::TryRegisteringBind(
			const CSourceLocation& location,
			const CStack& stack,
			const std::string& name,
			const CValue& value)
	{
		if (t_binds.find(name) != end(t_binds)) {
			throw ExScopeSymbolAlreadyRegistered{ location, stack };
		}
		RegisterBind(name, value);
	}

	void CScope::RegisterBind(
		const std::string& name,
		const CValue& value)
	{
		t_binds[name] = value;
	}

	const CValue& CGlobalScope::GetValue(const std::string& name)
	{
		if (t_binds.find(name) == end(t_binds)) {
			throw std::invalid_argument{ name };
		} else {
			return t_binds[name];
		}
	}

	std::vector<std::pair<std::string, CValue>>
	CLocalScope::FindNonGlobalValues(const std::vector<std::string>& names) const
	{
		std::vector<std::pair<std::string, CValue>> result;
		for(const std::string& name : names) {
			auto found = t_binds.find(name);
			if (found != end(t_binds)) {
				result.push_back(*found);
			}
		}
		return result;
	}

	const CValue& CLocalScope::GetValue(const std::string& name)
	{
		if (t_binds.find(name) == end(t_binds)) {
			return m_globalScope.GetValue(name);
		} else {
			return t_binds[name];
		}
	}

	// TODO: Clean up the below, store the locations with the captures.
	CValue CallFunction(
		CScope& scope,
		CStack& stack,
		const CSourceLocation& location,
		const std::string& symbol,
		const std::vector<CValue>& argValues)
	{
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

		const auto& captures = functionValue.GetFuncCaptures();

		if (argValues.size() < functionValue.GetFuncArity()) {
			return CValue::MakeFunction(&functionDef, captures, applArgs);
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
		for (const auto& capture : captures) {
			funcScope.TryRegisteringBind(
				CSourceLocation::MakeRegular(666, -666),
				stack,
				capture.first,
				capture.second);
		}

		// 3.3. Execute the function.
		stack.Push(symbol);
		CValue result = functionDef.Execute(funcScope, stack);
		stack.Pop();

		return result;
	}

}
}
