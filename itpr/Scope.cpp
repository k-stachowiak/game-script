#include "Scope.h"

#include <stdexcept>
#include <cassert>

#include "Exceptions.h"
#include "AstBind.h"
#include "AstLiteral.h"
#include "Stack.h"

namespace moon {
namespace itpr {

	CValue CScope::m_AcquireFunction(
		CStack& stack,
		const CSourceLocation& location,
		const std::string& symbol)
	{
		// TODO: Reduce the complexity of the below
		//       or rewrite to be more clear.

		const CAstBind* bind;
		try {
			bind = GetBind(symbol);
		}
		catch (const std::invalid_argument&) {
			throw ExScopeSymbolNotRegistered{ location, stack };
		}

		const CAstNode& function = bind->GetExpression();
		CValue functionValue = function.Evaluate(*this, stack);

		if (!IsFunction(functionValue)) {
			throw ExScopeSymbolIsNotFunction{ location, stack };
		}

		return functionValue;
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
			std::unique_ptr<CAstNode>&& expression)
	{
		if (m_bind_map.find(name) != end(m_bind_map)) {
			throw ExScopeSymbolAlreadyRegistered{ location, stack };
		}
		RegisterBind(location, name, std::move(expression));
	}

	void CScope::RegisterBind(
		const CSourceLocation& location,
		const std::string& name,
		std::unique_ptr<CAstNode>&& expression)
	{
		m_binds.push_back(std::unique_ptr<CAstBind> {
			new CAstBind{ location, name, std::move(expression) }
		});
		m_bind_map[name] = m_binds.back().get();
	}

	const CAstBind* CScope::GetBind(const std::string& name)
	{
		if (m_bind_map.find(name) == end(m_bind_map)) {
			if (!m_parent) {
				throw std::invalid_argument{ name };
			} else {
				return m_parent->GetBind(name);
			}
		}

		return m_bind_map[name];
	}

	CValue CScope::CallFunction(
		CStack& stack,
		const CSourceLocation& location,
		const std::string& symbol,
		const std::vector<CValue>& argValues)
	{
		// 1. Passed more than can be applied - error.
		// -------------------------------------------

		CValue functionValue = m_AcquireFunction(stack, location, symbol);
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
		CScope funcScope{ this };
		for (unsigned i = 0; i < argsCount; ++i) {
			std::unique_ptr<CAstNode> expression{ new CAstLiteral{ argLocations[i], applArgs[i] } };
			funcScope.TryRegisteringBind(argLocations[i], stack, argNames[i], std::move(expression));
		}

		// 3.3. Execute the function.
		stack.Push(symbol);
		CValue result = functionDef.Execute(funcScope, stack);
		stack.Pop();

		return result;
	}

}
}
