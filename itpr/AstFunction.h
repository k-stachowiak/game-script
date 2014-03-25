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

		CAstFunction(int line, int column) : CAstNode{ line, column } {}

		virtual const std::vector<std::string>& GetFormalArgs() const = 0;
		virtual const std::vector<std::pair<int, int>>& GetArgLocations() const = 0;
	};

}
}

#endif
