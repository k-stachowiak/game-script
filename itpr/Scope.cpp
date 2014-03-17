#include "Scope.h"

namespace moon {
namespace itpr {

	void CScope::RegisterValue(const std::string& name, itpr::CValue value)
	{
		if (m_binds.find(name) != end(m_binds)) {
			throw std::invalid_argument{ "Already contains bind of this name" };
		}
		m_binds[name] = value;
	}

	void CScope::RegisterFunction(const std::string& name, std::unique_ptr<itpr::CAstNode>&& expr)
	{
		if (m_funcDecls.find(name) != end(m_funcDecls)) {
			throw std::invalid_argument{ "Already contains function declaration of this name" };
		}
		m_funcDecls[name] = std::move(expr);
	}

	itpr::CValue CScope::GetBind(const std::string& name)
	{
		if (m_binds.find(name) == end(m_binds)) {
			throw std::invalid_argument{ "Bind of this name not found" };
		}

		return m_binds[name];
	}

	itpr::CAstNode& CScope::GetFunction(const std::string& name)
	{
		if (m_funcDecls.find(name) == end(m_funcDecls)) {
			throw std::invalid_argument{ "Function declaration of this name not found" };
		}

		return *(m_funcDecls[name]);
	}

}
}
