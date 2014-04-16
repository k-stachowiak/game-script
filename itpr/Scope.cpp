#include "Scope.h"

#include <stdexcept>
#include <cassert>

#include "Exceptions.h"
#include "AstBind.h"
#include "AstLiteral.h"
#include "Stack.h"

namespace moon {
namespace itpr {

	std::pair<CValue, CScope*> CScope::m_GetScopedBind(const std::string& name)
	{
		if (m_binds.find(name) == end(m_binds)) {
			if (!m_parent) {
				throw std::invalid_argument{ name };
			}
			else {
				return m_parent->m_GetScopedBind(name);
			}
		}

		return std::make_pair(m_binds[name], this);
	}

	std::pair<CValue, CScope*> CScope::m_AcquireFunction(
		CStack& stack,
		const CSourceLocation& location,
		const std::string& symbol)
	{
		// TODO: Reduce the complexity of the below
		//       or rewrite to be more clear.

		CValue functionValue;
		CScope* scope;
		try {
			std::tie(functionValue, scope) = m_GetScopedBind(symbol);
		}
		catch (const std::invalid_argument&) {
			throw ExScopeSymbolNotRegistered{ location, stack };
		}

		if (!IsFunction(functionValue)) {
			throw ExScopeSymbolIsNotFunction{ location, stack };
		}

		return std::make_pair(functionValue, scope);
	}

	CScope::CScope() :
		m_parent{ nullptr }
	{}

	CScope::CScope(CScope* parent) :
		m_parent{ parent }
	{}

	void CScope::TryRegisteringBind(
			const CSourceLocation& location,
			const CStack& stack,
			const std::string& name,
			const CValue& value)
	{
		if (m_binds.find(name) != end(m_binds)) {
			throw ExScopeSymbolAlreadyRegistered{ location, stack };
		}
		RegisterBind(name, value);
	}

	void CScope::RegisterBind(
		const std::string& name,
		const CValue& value)
	{
		m_binds[name] = value;
	}

	const CValue& CScope::GetValue(const std::string& name)
	{
		if (m_binds.find(name) == end(m_binds)) {
			if (!m_parent) {
				throw std::invalid_argument{ name };
			}
			else {
				return m_parent->GetValue(name);
			}
		}

		return m_binds[name];
	}

	CValue CScope::CallFunction(
		CStack& stack,
		const CSourceLocation& location,
		const std::string& symbol,
		const std::vector<CValue>& argValues)
	{
		// 1. Passed more than can be applied - error.
		// -------------------------------------------

		CValue functionValue;
		CScope* scope;
		std::tie(functionValue, scope) = m_AcquireFunction(stack, location, symbol);
		if (argValues.size() > functionValue.GetFuncArity()) {
			throw ExScopeFormalActualArgCountMismatch{
				location,
				stack
			};
		}

		// 2. Passed less than needed - curry on ;-).
		// ------------------------------------------

		const CAstFunction& functionDef = functionValue.GetFuncDef();
		std::vector<CValue> applArgs = functionValue.GetAppliedArgs();
		std::copy(begin(argValues), end(argValues), std::back_inserter(applArgs));

		if (argValues.size() < functionValue.GetFuncArity()) {
			return CValue::MakeFunction(&functionDef, applArgs);
		}

		// 3. Passed the exact amount needed for the execution.
		// ----------------------------------------------------

		// 3.1. Analyze args.
		const std::vector<std::string>& argNames = functionDef.GetFormalArgs();
		const std::vector<CSourceLocation>& argLocations = functionDef.GetArgLocations();
		assert(argNames.size() == argLocations.size());
		unsigned argsCount = argNames.size();

		// 3.2. Build local scope for the function.
		CScope funcScope{ scope };
		for (unsigned i = 0; i < argsCount; ++i) {
			funcScope.TryRegisteringBind(argLocations[i], stack, argNames[i], applArgs[i]);
		}

		// 3.3. Execute the function.
		stack.Push(symbol);
		CValue result = functionDef.Execute(funcScope, stack);
		stack.Pop();

		return result;
	}

}
}
