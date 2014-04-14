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
		std::map<std::string, CValue> m_binds;

	public:
		CScope();
		CScope(std::shared_ptr<CScope> parent);

		void TryRegisteringBind(
				const CSourceLocation& location,
				const CStack& stack,
				const std::string& name,
				const CValue& value);

		void RegisterBind(
			const std::string& name,
			CValue value);

		CValue GetValue(const std::string& name);
	};

}
}

#endif
