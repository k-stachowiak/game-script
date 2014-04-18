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

	

}
}
