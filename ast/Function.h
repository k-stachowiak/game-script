#ifndef MOON_AST_FUNCTION_H
#define MOON_AST_FUNCTION_H

#include <cassert>
#include <utility>

#include "Node.h"

namespace moon {
namespace ast {

	class CAstFunction : public CAstNode {
		const std::vector<std::string> m_formalArgs;
		const std::vector<CSourceLocation> m_argLocations;

	public:
		CAstFunction(
			const CSourceLocation& location,
			const std::vector<std::string>& formalArgs,
			const std::vector<CSourceLocation>& argLocations);

		CValue Evaluate(itpr::CScope& scope, itpr::CStack&) const;
		const std::vector<std::string>& GetFormalArgs() const;
		const std::vector<CSourceLocation>& GetArgLocations() const;
		int GetArgsCount() const;
		virtual CValue Execute(itpr::CScope& scope, itpr::CStack& stack) const = 0;
	};

}
}

#endif
