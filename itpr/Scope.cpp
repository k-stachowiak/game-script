#include "Scope.h"

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

	void CScope::RegisterBind(const std::string& name, std::unique_ptr<CAstNode>&& expression)
	{
		if (m_bind_map.find(name) != end(m_bind_map)) {
			throw std::invalid_argument{ "Already contains bind of this name" };
		}
		m_binds.push_back(std::unique_ptr<CAstBind> {
			new CAstBind{ name, std::move(expression) }
		});
		m_bind_map[name] = m_binds.back().get();
	}

	CAstBind* CScope::GetBind(const std::string& name)
	{
		if (m_bind_map.find(name) == end(m_bind_map)) {
			throw std::invalid_argument{ "Bind of this name not found" };
		}

		return m_bind_map[name];
	}

	CValue CallFunction(
		CScope& scope,
		const std::string& symbol,
		const std::vector<CValue>& args)
	{
		const CAstBind* bind;
		try {
			bind = scope.GetBind(symbol);
		} catch (const std::invalid_argument&) {
			throw std::invalid_argument{ "Symbol not found in this scope" };
		}

		const auto* function = bind->TryGettingFuncDecl();

		if (!function) {
			throw std::invalid_argument{ "Symbol not a function in this scope" };
		}

		std::vector<std::string> formalArgs = function->GetFormalArgs();

		if (formalArgs.size() != args.size()) {
			throw std::invalid_argument{ "Formal/actual argument count mismatch" };
		}

		CScope funcScope{ &scope };

		unsigned commonSize = args.size();
		for (unsigned i = 0; i < commonSize; ++i) {
			funcScope.RegisterBind(
				formalArgs[i],
				std::unique_ptr<CAstNode> {new CAstLiteral{ args[i] }});
		}

		return function->Evaluate(scope);
	}

}
}
