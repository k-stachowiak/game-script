#include "Scope.h"

#include "../except/Scope.h"
#include "AstBind.h"
#include "AstLiteral.h"

namespace moon {
namespace itpr {

	CScope::CScope() :
		m_parent(nullptr)
	{}

	CScope::CScope(CScope* parent) :
		m_parent{ parent }
	{}

	void CScope::RegisterBind(std::unique_ptr<CAstBind>&& bind)
	{
		std::string name = bind->GetSymbol();
		if (m_bind_map.find(name) != end(m_bind_map)) {
			throw except::ExScope::SymbolAlreadyRegistered{};
		}

		m_binds.push_back(std::move(bind));
		m_bind_map[name] = m_binds.back().get();
	}

	void CScope::RegisterBind(const std::string& name, std::unique_ptr<CAstNode>&& expression)
	{
		if (m_bind_map.find(name) != end(m_bind_map)) {
			throw except::ExScope::SymbolAlreadyRegistered{};
		}
		m_binds.push_back(std::unique_ptr<CAstBind> {
			new CAstBind{ name, std::move(expression) }
		});
		m_bind_map[name] = m_binds.back().get();
	}

	CAstBind* CScope::GetBind(const std::string& name)
	{
		if (m_bind_map.find(name) == end(m_bind_map)) {
			if (!m_parent) {
				throw except::ExScope::SymbolNotRegistered{};
			} else {
				return m_parent->GetBind(name);
			}
		}

		return m_bind_map[name];
	}

	CValue CScope::CallFunction(
		const std::string& symbol,
		const std::vector<CValue>& args)
	{
		const CAstBind* bind;
		try {
			bind = GetBind(symbol);
		}
		catch (const std::invalid_argument&) {
			throw except::ExScope::SymbolNotRegistered{};
		}

		const auto* function = bind->TryGettingFuncDecl();
		if (!function) {
			throw except::ExScope::SymbolIsNotFunction{};
		}

		std::vector<std::string> formalArgs = function->GetFormalArgs();
		if (formalArgs.size() != args.size()) {
			throw except::ExScope::FormalActualArgCountMismatch{};
		}

		CScope funcScope{ this };
		for (unsigned i = 0; i < args.size(); ++i) {
			funcScope.RegisterBind(
				formalArgs[i],
				std::unique_ptr<CAstNode> {new CAstLiteral{ args[i] }}
			);
		}

		return function->Evaluate(funcScope);
	}

}
}
