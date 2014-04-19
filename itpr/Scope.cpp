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
			const CStack& stack,
			const std::string& name,
			const CValue& value,
			const CSourceLocation& location)
	{
		if (t_binds.find(name) != end(t_binds)) {
			throw ExScopeSymbolAlreadyRegistered{ location, stack };
		}
		t_binds[name] = { value, location };
	}

	CValue CScope::GetValue(
		const std::string& name,
		const CSourceLocation& location,
		const CStack& stack)
	{
		return GetValueStore(name, location, stack).value;
	}

	CSourceLocation CScope::GetLocation(
		const std::string& name,
		const CSourceLocation& location,
		const CStack& stack)
	{
		return GetValueStore(name, location, stack).location;
	}

	const SValueStore CGlobalScope::GetValueStore(
		const std::string& name,
		const CSourceLocation& location,
		const CStack& stack) const
	{
		if (t_binds.find(name) == end(t_binds)) {
			throw ExScopeSymbolNotRegistered{ location, stack };
		} else {
			return t_binds.at(name);
		}
	}

	const SValueStore CLocalScope::GetValueStore(
		const std::string& name,
		const CSourceLocation& location,
		const CStack& stack) const
	{
		if (t_binds.find(name) == end(t_binds)) {
			return m_globalScope.GetValueStore(name, location, stack);
		}
		else {
			return t_binds.at(name);
		}
	}

	void CLocalScope::FindNonGlobalValues(
		const std::vector<std::string>& in_names,
		std::vector<std::string>& names,
		std::vector<CValue>& values,
		std::vector<CSourceLocation>& locations) const
	{
		std::vector<std::pair<std::string, CValue>> result;
		for(const std::string& in_name : in_names) {
			auto found = t_binds.find(in_name);
			if (found == end(t_binds)) {
				continue;
			} else {
				names.push_back(found->first);
				values.push_back(found->second.value);
				locations.push_back(found->second.location);
			}
		}
	}

}
}
