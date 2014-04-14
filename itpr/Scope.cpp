#include "Scope.h"

#include <memory>
#include <stdexcept>
#include <cassert>

#include "Exceptions.h"
#include "AstBind.h"
#include "AstLiteral.h"
#include "Stack.h"

namespace moon {
namespace itpr {

	CScope::CScope() :
		m_parent{ nullptr }
	{}

	CScope::CScope(std::shared_ptr<CScope> parent) :
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
		CValue value)
	{
		m_binds[name] = value;
	}

	CValue CScope::GetValue(const std::string& name)
	{
		if (m_binds.find(name) == end(m_binds)) {
			if (!m_parent) {
				// TODO: Wat?
				throw std::invalid_argument{ name };
			}
			else {
				return m_parent->GetValue(name);
			}
		}

		return m_binds[name];
	}

}
}
