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

	std::map<std::string, SCapture>
	CLocalScope::CaptureNonGlobals(const std::vector<std::string>& names) const
	{
		std::map<std::string, SCapture> result;
		for(const std::string& name : names) {
			auto found = t_binds.find(name);
			if (found == end(t_binds)) {
				continue;
			} else {
				result[name] = {
					found->second.value,
					found->second.location
				};
			}
		}
		return result;
	}

}
}
