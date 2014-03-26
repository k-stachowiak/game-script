#ifndef MOON_EXCEPT_AST_H
#define MOON_EXCEPT_AST_H

#include <stdexcept>

namespace moon {
namespace except {

	struct ExAst {
		struct ReferenceToFunctionEvaluated : public ExCompilationError {
			ReferenceToFunctionEvaluated(int line, int column) :
				ExCompilationError{ line, column, "Reference to function evaluated." }
			{}
		};
		struct ArithmeticTypeMismatch : public ExCompilationError {
			ArithmeticTypeMismatch(int line, int column) :
				ExCompilationError{ line, column, "Arithmetic type mismatch." }
			{}
		};
	};

}
}

#endif
