#ifndef MOON_ITPR_FUNCTION_H
#define MOON_ITPR_FUNCTION_H

#include <cassert>
#include <utility>

#include "AstNode.h"

namespace moon {
namespace itpr {

	class CAstFunction : public CAstNode {
		const std::vector<std::string> m_formalArgs;
		const std::vector<CSourceLocation> m_argLocations;

	public:
		// Note: This class only exists as a common base for the AST function
		// Definitions and built-in functions to be universally and seamlesly
		// accessible from the scope.

		CAstFunction(
			const CSourceLocation& location,
			const std::vector<std::string>& formalArgs,
			const std::vector<CSourceLocation>& argLocations) :
				CAstNode{ location },
				m_formalArgs{ formalArgs },
				m_argLocations{ argLocations }
		{
			assert(m_formalArgs.size() == m_argLocations.size());
		}

		CValue Evaluate(CScope&, CStack&) const
		{
			return CValue::MakeFunction(this, {});
		}

		const std::vector<std::string>& GetFormalArgs() const
		{
			return m_formalArgs;
		}

		const std::vector<CSourceLocation>& GetArgLocations() const
		{
			return m_argLocations;
		}

		int GetArgsCount() const
		{
			return m_formalArgs.size();
		}

		virtual CValue Execute(CScope& scope, CStack& stack) const = 0;
	};

}
}

#endif
