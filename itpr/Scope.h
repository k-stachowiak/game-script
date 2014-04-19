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
		// TODO: Join the two maps.
		std::map<std::string, CValue> t_binds;
		std::map<std::string, CSourceLocation> t_locations;
	public:
		virtual ~CScope() {}

		void TryRegisteringBind(
			const CSourceLocation& location, // TODO: reorder args here.
			const CStack& stack,
			const std::string& name,
			const CValue& value);

		virtual CGlobalScope& GetGlobalScope() = 0;

		virtual void FindNonGlobalValues(
			const std::vector<std::string>& in_names,
			std::vector<std::string>& names,
			std::vector<CValue>& values,
			std::vector<CSourceLocation>& locations) const = 0;

		virtual std::pair<CValue, CSourceLocation>
		GetValueLocation(const std::string& name) = 0;

		CValue GetValue(const std::string& name);
		CSourceLocation GetLocation(const std::string& name);
	};

	class CGlobalScope : public CScope {
	public:
		CGlobalScope& GetGlobalScope() override { return *this; }

		void FindNonGlobalValues(
			const std::vector<std::string>&,
			std::vector<std::string>&,
			std::vector<CValue>&,
			std::vector<CSourceLocation>&) const override
		{}

		std::pair<CValue, CSourceLocation>
		GetValueLocation(const std::string& name) override;
	};

	class CLocalScope : public CScope {
		CGlobalScope& m_globalScope;

	public:
		CLocalScope(CGlobalScope& globalScope) : m_globalScope(globalScope) {}

		CGlobalScope& GetGlobalScope() override { return m_globalScope; }

		void FindNonGlobalValues(
			const std::vector<std::string>& in_names,
			std::vector<std::string>& names,
			std::vector<CValue>& values,
			std::vector<CSourceLocation>& locations) const override;

		std::pair<CValue, CSourceLocation>
		GetValueLocation(const std::string& name) override;
	};


}
}

#endif
