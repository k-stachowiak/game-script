#include "Scope.h"

#include <stdexcept>
#include <cassert>

#include "Exceptions.h"
#include "AstBind.h"
#include "AstLiteral.h"
#include "Stack.h"

namespace moon {
namespace itpr {

	const CAstFunction& CScope::m_AcquireFunction(
		CStack& stack,
		const CSourceLocation& location,
		const std::string& symbol)
	{
		const CAstBind* bind;
		try {
			bind = GetBind(symbol);
		}
		catch (const std::invalid_argument&) {
			throw ExScopeSymbolNotRegistered{ location, stack };
		}

		const CAstFunction* function = bind->TryGettingFunction();
		if (!function) {
			throw ExScopeSymbolIsNotFunction{ location, stack };
		}

		return *function;
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
		// TODO:
		// - have the m_AcquireFunction take into account the function values and already applied
		// - If not function value, the nothing already applied
		// - Rewrite the rest of the function so that it is rather a procedure of building up the
		//   already applied list and only at the end the real call takes place.
		//
		// Wonder: What is the difference between a call to the global function and a function value?
		//         How to express that in the source code?
		//
		// Note: THE SCOPE CAN NOW RETURN "name -> value" PAIRS SINCE THE FUNCTION VALUES HAVE BEEN CREATED!
		const CAstFunction& function = m_AcquireFunction(stack, location, symbol);

		const std::vector<std::string>& argNames = function.GetFormalArgs();
		const std::vector<CSourceLocation>& argLocations = function.GetArgLocations();
		assert(argNames.size() == argLocations.size());
		unsigned argsCount = argNames.size();

		if (argValues.size() > argsCount) {
			throw ExScopeFormalActualArgCountMismatch{
				location,
				stack
			};
		}

		if (argsCount == argValues.size()) {
			CScope funcScope{ this };
			for (unsigned i = 0; i < argsCount; ++i) {
				std::unique_ptr<CAstNode> expression{ new CAstLiteral{ argLocations[i], argValues[i] } };
				funcScope.TryRegisteringBind(argLocations[i], stack, argNames[i], std::move(expression));
			}

			stack.Push(symbol);
			CValue result = function.Execute(funcScope, stack);
			stack.Pop();

			return result;
		} else {
			// Note: This case could be detected as soon as possible
			// for the sake of the optimization.
			return CValue::MakeFunction(&function, argValues);
		}
	}

}
}
