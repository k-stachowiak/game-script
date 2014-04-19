#ifndef MOON_ITPR_FUNCTION_H
#define MOON_ITPR_FUNCTION_H

#include <cassert>
#include <utility>

#include "AstNode.h"

namespace moon {
namespace itpr {

	class CAstFunction : public CAstNode {
		// TODO: unify this somehow.
		const std::vector<std::string> m_formalArgs;
		const std::vector<CSourceLocation> m_argLocations;

	public:
		CAstFunction(
			const CSourceLocation& location,
			const std::vector<std::string>& formalArgs,
			const std::vector<CSourceLocation>& argLocations);

		CValue Evaluate(CScope& scope, CStack&) const;
		const std::vector<std::string>& GetFormalArgs() const;
		const std::vector<CSourceLocation>& GetArgLocations() const;
		int GetArgsCount() const;
		virtual CValue Execute(CScope& scope, CStack& stack) const = 0;
	};

}
}

#endif
