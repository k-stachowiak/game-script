#ifndef MOON_ITPR_FUNCTION_H
#define MOON_ITPR_FUNCTION_H

#include "AstNode.h"

namespace moon {
namespace itpr {

	class CAstFunction : public CAstNode {
	public:
		// Note: This class only exists as a common base for the AST function
		// Definitions and built-in functions to be universally and seamlesly
		// accessible from the scope.

		virtual const std::vector<std::string>& GetFormalArgs() const = 0;
	};

}
}

#endif