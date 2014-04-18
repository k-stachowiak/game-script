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

	class CGlobalScope;

	class CScope {
	protected:
		std::map<std::string, CValue> t_binds;
	public:
		virtual ~CScope() {}

		void TryRegisteringBind(
			const CSourceLocation& location,
			const CStack& stack,
			const std::string& name,
			const CValue& value);

		void RegisterBind(
			const std::string& name,
			const CValue& value);

		virtual CGlobalScope& GetGlobalScope() = 0;

		virtual std::vector<std::pair<std::string, CValue>>
		FindNonGlobalValues(const std::vector<std::string>& names) const = 0;

		virtual const CValue& GetValue(const std::string& name) = 0;
	};

	class CGlobalScope : public CScope {
	public:
		CGlobalScope& GetGlobalScope() { return *this; }

		std::vector<std::pair<std::string, CValue>>
		FindNonGlobalValues(const std::vector<std::string>&) const
		{
			return {};
		}

		const CValue& GetValue(const std::string& name);
	};

	class CLocalScope : public CScope {
		CGlobalScope& m_globalScope;

	public:
		CLocalScope(CGlobalScope& globalScope) : m_globalScope(globalScope) {}

		CGlobalScope& GetGlobalScope() { return m_globalScope; }

		std::vector<std::pair<std::string, CValue>>
		FindNonGlobalValues(const std::vector<std::string>& names) const;

		const CValue& GetValue(const std::string& name);
	};


}
}

#endif
