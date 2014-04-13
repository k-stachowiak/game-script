#ifndef MOON_ITPR_SCOPE_H
#define MOON_ITPR_SCOPE_H

#include <memory>
#include <string>
#include <utility>
#include <map>

#include "../API/Value.h"
#include "../common/SourceLocation.h"

#include "AstBind.h"

namespace moon {
namespace itpr {

	class CScope : public std::enable_shared_from_this<CScope> {
		std::shared_ptr<CScope> m_parent;
		std::vector<std::unique_ptr<CAstBind>> m_binds;
		std::map<std::string, CAstBind*> m_bind_map;

		std::pair<const CAstBind*, std::shared_ptr<CScope>> m_GetScopedBind(const std::string& name);

		std::pair<CValue, std::shared_ptr<CScope>> m_AcquireFunction(
			CStack& stack,
			const CSourceLocation& location,
			const std::string& symbol);

	public:
		CScope();
		CScope(const std::shared_ptr<CScope>& parent);

		void TryRegisteringBind(
				const CSourceLocation& location,
				const CStack& stack,
				const std::string& name,
				std::unique_ptr<CAstNode>&& expression);

		void RegisterBind(
			const CSourceLocation& location,
			const std::string& name,
			std::unique_ptr<CAstNode>&& expression);

		const CAstBind* GetBind(const std::string& name);

		CValue CallFunction(
				CStack& stack,
				const CSourceLocation& location,
				const std::string& symbol,
				const std::vector<CValue>& argValues);
	};

}
}

#endif
