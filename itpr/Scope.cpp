#include "Scope.h"

#include "../except/Scope.h"
#include "AstBind.h"
#include "AstLiteral.h"

namespace moon {
namespace itpr {

	CScope::CScope() :
		m_parent{ nullptr }
	{}

	CScope::CScope(CScope* parent) :
		m_parent{ parent }
	{}

	void CScope::RegisterBind(
			int line,
			int column,
			const std::string& name,
			std::unique_ptr<CAstNode>&& expression)
	{
		if (m_bind_map.find(name) != end(m_bind_map)) {
			throw except::ExScope::SymbolAlreadyRegistered{};
		}
		m_binds.push_back(std::unique_ptr<CAstBind> {
			new CAstBind{ line, column, name, std::move(expression) }
		});
		m_bind_map[name] = m_binds.back().get();
	}

	const CAstBind* CScope::GetBind(const std::string& name)
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
		std::vector<std::pair<int, int>> argLocations = function->GetArgLocations();
		if (formalArgs.size() != args.size() || argLocations.size() != args.size()) {
			throw except::ExScope::FormalActualArgCountMismatch{};
		}

		CScope funcScope{ this };
		for (unsigned i = 0; i < args.size(); ++i) {
			funcScope.RegisterBind(
				argLocations[i].first,
				argLocations[i].second,
				formalArgs[i],
				std::unique_ptr<CAstNode> {
					new CAstLiteral{
						argLocations[i].first,
						argLocations[i].second,
						args[i]
					}
				}
			);
		}

		return function->Evaluate(funcScope);
	}

}
}
