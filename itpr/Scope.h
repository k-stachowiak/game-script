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

	struct SValueStore {
		CValue value;
		CSourceLocation location;
	};

	class CScope {
	protected:
		std::map<std::string, SValueStore> t_binds;
	public:
		virtual ~CScope() {}

		virtual const SValueStore GetValueStore(
			const std::string& name,
			const CSourceLocation& location,
			const CStack& stack) const = 0;

		virtual CGlobalScope& GetGlobalScope() = 0;

		virtual void FindNonGlobalValues(
			const std::vector<std::string>& in_names,
			std::vector<std::string>& names,
			std::vector<CValue>& values,
			std::vector<CSourceLocation>& locations) const = 0;

		void TryRegisteringBind(
			const CStack& stack,
			const std::string& name,
			const CValue& value,
			const CSourceLocation& location);

		CValue GetValue(
			const std::string& name,
			const CSourceLocation& location,
			const CStack& stack);

		CSourceLocation GetLocation(
			const std::string& name,
			const CSourceLocation& location,
			const CStack& stack);
	};

	class CGlobalScope : public CScope {
	public:
		const SValueStore GetValueStore(
			const std::string& name,
			const CSourceLocation& location,
			const CStack& stack) const override;

		CGlobalScope& GetGlobalScope() override { return *this; }

		void FindNonGlobalValues(
			const std::vector<std::string>&,
			std::vector<std::string>&,
			std::vector<CValue>&,
			std::vector<CSourceLocation>&) const override
		{}
	};

	class CLocalScope : public CScope {
		CGlobalScope& m_globalScope;

	public:
		CLocalScope(CGlobalScope& globalScope) : m_globalScope(globalScope) {}
		
		const SValueStore GetValueStore(
			const std::string& name,
			const CSourceLocation& location,
			const CStack& stack) const override;

		CGlobalScope& GetGlobalScope() override { return m_globalScope; }

		void FindNonGlobalValues(
			const std::vector<std::string>& in_names,
			std::vector<std::string>& names,
			std::vector<CValue>& values,
			std::vector<CSourceLocation>& locations) const override;
	};


}
}

#endif
