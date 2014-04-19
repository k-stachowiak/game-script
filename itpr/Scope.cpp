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
		t_binds[name] = value;
		t_locations[name] = location;
	}

	CValue CScope::GetValue(const std::string& name)
	{
		CValue value;
		CSourceLocation location;
		std::tie(value, location) = GetValueLocation(name);
		return value;
	}

	CSourceLocation CScope::GetLocation(const std::string& name)
	{
		CValue value;
		CSourceLocation location;
		std::tie(value, location) = GetValueLocation(name);
		return location;
	}

	std::pair<CValue, CSourceLocation>
	CGlobalScope::GetValueLocation(const std::string& name)
	{
		if (t_binds.find(name) == end(t_binds)) {
			throw std::invalid_argument{ name };
		} else {
			return std::make_pair(t_binds[name], t_locations[name]);
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
			if (t_binds.find(in_name) == end(t_binds)) {
				continue;
			} else {
				names.push_back(in_name);
				values.push_back(t_binds.at(in_name));
				locations.push_back(t_locations.at(in_name));
			}
		}
	}

	std::pair<CValue, CSourceLocation>
	CLocalScope::GetValueLocation(const std::string& name)
	{
		if (t_binds.find(name) == end(t_binds)) {
			return m_globalScope.GetValueLocation(name);
		} else {
			return std::make_pair(t_binds[name], t_locations[name]);
		}
	}

}
}
