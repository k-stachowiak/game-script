#ifndef MOON_EXCEPT_AST_H
#define MOON_EXCEPT_AST_H

#include <stdexcept>

namespace moon {
namespace except {

	struct ExAst {
		struct ReferenceToFunctionEvaluated : public std::runtime_error {
			ReferenceToFunctionEvaluated() :
				std::runtime_error{ "Reference to function evaluated." }
			{}
		};
		struct ArithmeticTypeMismatch : public std::runtime_error {
			ArithmeticTypeMismatch() :
				std::runtime_error{ "Arithmetic type mismatch." }
			{}
		};
	};

}
}

#endif