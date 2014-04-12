#ifndef MOON_ITPR_FUNCTION_H
#define MOON_ITPR_FUNCTION_H

#include <utility>

#include "AstNode.h"

namespace moon {
namespace itpr {

	class CAstFunction : public CAstNode {
	public:
		// Note: This class only exists as a common base for the AST function
		// Definitions and built-in functions to be universally and seamlesly
		// accessible from the scope.

		CAstFunction(const CSourceLocation& location) : CAstNode{ location } {}

		CValue Evaluate(CScope&, CStack&) const
		{
			return CValue::MakeFunction(this, {});
		}

		virtual CValue Execute(CScope& scope, CStack& stack) const = 0;
		virtual const std::vector<std::string>& GetFormalArgs() const = 0;
		virtual const std::vector<CSourceLocation>& GetArgLocations() const = 0;
		virtual int GetArgsCount() const = 0;
	};

}
}

#endif
