#include "Scope.h"

#include "AstFuncDecl.h"

namespace moon {
namespace itpr {

	CScope::CScope() :
		m_parent(nullptr)
	{}

	CScope::CScope(CScope* parent) :
		m_parent{ parent }
	{}

	void CScope::RegisterValue(const std::string& name, CValue value)
	{
		if (m_binds.find(name) != end(m_binds)) {
			throw std::invalid_argument{ "Already contains bind of this name" };
		}
		m_binds[name] = value;
	}

	void CScope::RegisterFunction(const std::string& name, std::unique_ptr<itpr::CAstFuncDecl>&& expr)
	{
		if (m_funcDecls.find(name) != end(m_funcDecls)) {
			throw std::invalid_argument{ "Already contains function declaration of this name" };
		}
		m_funcDecls[name] = std::move(expr);
	}

	CValue CScope::GetBind(const std::string& name)
	{
		if (m_binds.find(name) == end(m_binds)) {
			throw std::invalid_argument{ "Bind of this name not found" };
		}

		return m_binds[name];
	}

	itpr::CAstFuncDecl& CScope::GetFunction(const std::string& name)
	{
		if (m_funcDecls.find(name) == end(m_funcDecls)) {
			throw std::invalid_argument{ "Function declaration of this name not found" };
		}

		return *(m_funcDecls[name]);
	}

	CValue g_CallFunction(
		CScope& scope,
		itpr::CAstFuncDecl& function,
		const std::vector<CValue>& args)
	{
		std::vector<std::string> formalArgs = function.GetFormalArgs();

		if (formalArgs.size() != args.size()) {
			throw std::invalid_argument{ "Formal/actual argument count mismatch" };
		}

		CScope funcScope{ &scope };

		unsigned commonSize = args.size();
		for (unsigned i = 0; i < commonSize; ++i) {
			funcScope.RegisterValue(formalArgs[i], args[i]);
		}

		return function.Evaluate(scope);
	}

}
}
