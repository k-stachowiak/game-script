#include "Scope.h"

#include <stdexcept>

#include "Exceptions.h"
#include "AstBind.h"
#include "AstLiteral.h"
#include "Stack.h"

namespace moon {
namespace itpr {

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
		const std::vector<CValue>& args)
	{
		const CAstBind* bind;
		try {
			bind = GetBind(symbol);
		}
		catch (const std::invalid_argument&) {
			throw ExScopeSymbolNotRegistered{ location, stack };
		}

		const auto* function = bind->TryGettingFunction();
		if (!function) {
			throw ExScopeSymbolIsNotFunction{ location, stack };
		}

		const std::vector<std::string>& formalArgs = function->GetFormalArgs();
		const std::vector<CSourceLocation>& argLocations = function->GetArgLocations();
		if (formalArgs.size() != args.size() || argLocations.size() != args.size()) {
			throw ExScopeFormalActualArgCountMismatch{ location, stack };
		}

		CScope funcScope{ this };
		for (unsigned i = 0; i < args.size(); ++i) {
			funcScope.TryRegisteringBind(
				argLocations[i],
				stack,
				formalArgs[i],
				std::unique_ptr<CAstNode> {
					new CAstLiteral{
						argLocations[i],
						args[i]
					}
				}
			);
		}

		stack.Push(symbol);
		CValue result = function->Evaluate(funcScope, stack);
		stack.Pop();

		return result;
	}

}
}
