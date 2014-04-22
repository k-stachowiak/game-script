#ifndef MOON_ITPR_SCOPE_H
#define MOON_ITPR_SCOPE_H

#include <memory>
#include <string>
#include <utility>
#include <map>

#include "Stack.h"

#include "../API/Value.h"
#include "../common/SourceLocation.h"
#include "../ast/Bind.h"

namespace moon {
namespace itpr {

	class CGlobalScope;

	// TODO: this is identical to the SCapture. Merge the two?
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

		virtual std::map<std::string, SCapture>
		CaptureNonGlobals(const std::vector<std::string>& names) const = 0;

		void TryRegisteringBind(
			const CStack& stack,
			const std::string& name,
			const CValue& value,
			const CSourceLocation& location);

		std::vector<std::string> GetAllValues() const;
		std::vector<std::string> GetAllFunctions() const;

		CValue GetValue(
			const std::string& name,
			const CSourceLocation& location,
			const CStack& stack);
	};

	class CGlobalScope : public CScope {
	public:
		CGlobalScope& GetGlobalScope() override { return *this; }

		const SValueStore GetValueStore(
			const std::string& name,
			const CSourceLocation& location,
			const CStack& stack) const override;

		std::map<std::string, SCapture>
		CaptureNonGlobals(const std::vector<std::string>&) const override
		{
			return {};
		}
	};

	class CLocalScope : public CScope {
		CGlobalScope& m_globalScope;

	public:
		CLocalScope(CGlobalScope& globalScope) : m_globalScope(globalScope) {}
		
		CGlobalScope& GetGlobalScope() override { return m_globalScope; }

		const SValueStore GetValueStore(
			const std::string& name,
			const CSourceLocation& location,
			const CStack& stack) const override;

		std::map<std::string, SCapture>
		CaptureNonGlobals(const std::vector<std::string>& names) const override;
	};


}
}

#endif
